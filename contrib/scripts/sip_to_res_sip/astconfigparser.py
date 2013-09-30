import re

from astdicts import OrderedDict
from astdicts import MultiOrderedDict

def merge_values(left, right, key):
    """Merges values from right into left."""
    if isinstance(left, list):
        vals0 = left
    else: # assume dictionary
        vals0 = left[key] if key in left else []
    vals1 = right[key] if key in right else []

    return vals0 + [i for i in vals1 if i not in vals0]

###############################################################################

class Section(MultiOrderedDict):
    """A Section is a MultiOrderedDict itself that maintains a list of
       key/value options.  However, in the case of an Asterisk config
       file a section may have other defaults sections that is can pull
       data from (i.e. templates).  So when an option is looked up by key
       it first checks the base section and if not found looks in the
       added default sections. If not found at that point then a 'KeyError'
       exception is raised.
    """
    count = 0

    def __init__(self, defaults=None, templates=None):
        MultiOrderedDict.__init__(self)
        # track an ordered id of sections
        Section.count += 1
        self.id = Section.count
        self._defaults = [] if defaults is None else defaults
        self._templates = [] if templates is None else templates

    def __cmp__(self, other):
        return cmp(self.id, other.id)

    def get(self, key, from_self=True, from_templates=True, from_defaults=True):
        if from_self and key in self:
            return MultiOrderedDict.__getitem__(self, key)

        if from_templates:
            if self in self._templates:
                return []
            for t in self._templates:
                try:
                    # fail if not found on the search - doing it this way
                    # allows template's templates to be searched.
                    return t.get(key, True, from_templates, from_defaults)
                except KeyError:
                    pass

        if from_defaults:
            for d in self._defaults:
                try:
                    return d.get(key, True, from_templates, from_defaults)
                except KeyError:
                    pass

        raise KeyError(key)

    def __getitem__(self, key):
        """Get the value for the given key. If it is not found in the 'self'
           then check inside templates and defaults before declaring raising
           a KeyError exception.
        """
        return self.get(key)

    def keys(self, self_only=False):
        res = MultiOrderedDict.keys(self)
        if self_only:
            return res

        for d in self._templates:
            for key in d.keys():
                if key not in res:
                    res.append(key)

        for d in self._defaults:
            for key in d.keys():
                if key not in res:
                    res.append(key)
        return res

    def add_defaults(self, defaults):
        defaults.sort()
        for i in defaults:
            self._defaults.insert(0, i)

    def add_templates(self, templates):
        templates.sort(reverse=True);
        self._templates.extend(templates)

    def get_merged(self, key):
        """Return a list of values for a given key merged from default(s)"""
        # first merge key/values from defaults together
        merged = []
        for i in reversed(self._defaults):
            if not merged:
                merged = i
                continue
            merged = merge_values(merged, i, key)

        for i in reversed(self._templates):
            if not merged:
                merged = i
                continue
            merged = merge_values(merged, i, key)

        # then merge self in
        return merge_values(merged, self, key)

###############################################################################

COMMENT = ';'
COMMENT_START = ';--'
COMMENT_END = '--;'

DEFAULTSECT = 'general'

def remove_comment(line, is_comment):
    """Remove any commented elements from the line."""
    if not line: return line, is_comment

    if is_comment:
        part = line.partition(COMMENT_END)
        if part[1]:
            # found multi-line comment end check string after it
            return remove_comment(part[2], False)
        return "", True

    part = line.partition(COMMENT_START)
    if part[1]:
        # found multi-line comment start check string before
        # it to make sure there wasn't an eol comment in it
        has_comment = part[0].partition(COMMENT)
        if has_comment[1]:
            # eol comment found return anything before it
            return has_comment[0], False

        # check string after it to see if the comment ends
        line, is_comment = remove_comment(part[2], True)
        if is_comment:
            # return possible string data before comment
            return part[0].strip(), True

        # otherwise it was an embedded comment so combine
        return ''.join([part[0].strip(), ' ', line]).rstrip(), False

    # check for eol comment
    return line.partition(COMMENT)[0].strip(), False

def try_include(line):
    """Checks to see if the given line is an include.  If so return the
       included filename, otherwise None.
    """
    if not line.startswith('#'):
        return None

    # it is an include - get file name
    try:
        return line[line.index('"') + 1:line.rindex('"')]
    except ValueError:
        print "Invalid include - could not parse filename."
        return None

def try_section(line):
    """Checks to see if the given line is a section. If so return the section
       name, otherwise return 'None'.
    """
    # leading spaces were stripped when checking for comments
    if not line.startswith('['):
        return None, False, []

    section, delim, templates = line.partition(']')
    if not templates:
        return section[1:], False, []

    # strip out the parens and parse into an array
    templates = templates.replace('(', "").replace(')', "").split(',')
    # go ahead and remove extra whitespace
    templates = [i.strip() for i in templates]
    try:
        templates.remove('!')
        return section[1:], True, templates
    except:
        return section[1:], False, templates

def try_option(line):
    """Parses the line as an option, returning the key/value pair."""
    data = re.split('=>?', line)
    # should split in two (key/val), but either way use first two elements
    return data[0].rstrip(), data[1].lstrip()

###############################################################################

def find_value(sections, key):
    """Given a list of sections, try to find value(s) for the given key."""
    # always start looking in the last one added
    sections.sort(reverse=True);
    for s in sections:
        try:
            # try to find in section and section's templates
            return s.get(key, from_defaults=False)
        except KeyError:
            pass

    # wasn't found in sections or a section's templates so check in defaults
    for s in sections:
        try:
            # try to find in section's defaultsects
            return s.get(key, from_self=False, from_templates=False)
        except KeyError:
            pass

    raise KeyError(key)

def find_dict(mdicts, key, val):
    """Given a list of mult-dicts, return the multi-dict that contains
       the given key/value pair."""

    def found(d):
        return key in d and val in d[key]

    try:
        return [d for d in mdicts if found(d)][0]
    except IndexError:
        raise LookupError("Dictionary not located for key = %s, value = %s"
                          % (key, val))

def get_sections(parser, key, attr='_sections', searched=None):
    if searched is None:
        searched = []
    if parser is None or parser in searched:
        return []

    try:
        sections = getattr(parser, attr)
        res = sections[key] if key in sections else []
        searched.append(parser)
        return res + get_sections(parser._includes, key, attr, searched) \
            + get_sections(parser._parent, key, attr, searched)
    except:
        # assume ordereddict of parsers
        res = []
        for p in parser.itervalues():
            res.extend(get_sections(p, key, attr, searched))
        return res

def get_defaults(parser, key):
    return get_sections(parser, key, '_defaults')

def write_dicts(file, mdicts):
    for section, sect_list in mdicts.iteritems():
        # every section contains a list of dictionaries
        for sect in sect_list:
            file.write("[%s]\n" % section)
            for key, val_list in sect.iteritems():
                # every value is also a list
                for v in val_list:
                    key_val = key
                    if v is not None:
                        key_val += " = " + str(v)
                        file.write("%s\n" % (key_val))
            file.write("\n")

###############################################################################

class MultiOrderedConfigParser:
    def __init__(self, parent=None):
        self._parent = parent
        self._defaults = MultiOrderedDict()
        self._sections = MultiOrderedDict()
        self._includes = OrderedDict()

    def defaults(self):
        return self._defaults

    def default(self, key):
        """Retrieves a list of dictionaries for a default section."""
        return get_defaults(self, key)

    def add_default(self, key, template_keys=None):
        """Adds a default section to defaults, returning the
           default Section object.
        """
        if template_keys is None:
            template_keys = []
        return self.add_section(key, template_keys, self._defaults)

    def sections(self):
        return self._sections

    def section(self, key):
        """Retrieves a list of dictionaries for a section."""
        return get_sections(self, key)

    def add_section(self, key, template_keys=None, mdicts=None):
        if template_keys is None:
            template_keys = []
        if mdicts is None:
            mdicts = self._sections
        res = Section()
        for t in template_keys:
            res.add_templates(get_defaults(self, t))
        res.add_defaults(get_defaults(self, DEFAULTSECT))
        mdicts.insert(0, key, res)
        return res

    def includes(self):
        return self._includes

    def add_include(self, filename, parser=None):
        if filename in self._includes:
            return self._includes[filename]

        self._includes[filename] = res = \
             MultiOrderedConfigParser(self) if parser is None else parser
        return res;

    def get(self, section, key):
        """Retrieves the list of values from a section for a key."""
        try:
            # search for the value in the list of sections
            return find_value(self.section(section), key)
        except KeyError:
            pass

        try:
            # section may be a default section so, search
            # for the value in the list of defaults
            return find_value(self.default(section), key)
        except KeyError:
            raise LookupError("key %r not found for section %r"
                              % (key, section))

    def set(self, section, key, val):
        """Sets an option in the given section."""
        # TODO - set in multiple sections? (for now set in first)
        # TODO - set in both sections and defaults?
        if section in self._sections:
            self.section(section)[0][key] = val
        else:
            self.defaults(section)[0][key] = val

    def read(self, filename):
        try:
            with open(filename, 'rt') as file:
                self._read(file, filename)
        except IOError:
            print "Could not open file ", filename, " for reading"

    def _read(self, file, filename):
        is_comment = False # used for multi-lined comments
        for line in file:
            line, is_comment = remove_comment(line, is_comment)
            if not line:
                # line was empty or was a comment
                continue

            include_name = try_include(line)
            if include_name:
                parser = self.add_include(include_name)
                parser.read(include_name)
                continue

            section, is_template, templates = try_section(line)
            if section:
                if section == DEFAULTSECT or is_template:
                    sect = self.add_default(section, templates)
                else:
                    sect = self.add_section(section, templates)
                continue

            key, val = try_option(line)
            sect[key] = val

    def write(self, f):
        try:
            for key, val in self._includes.iteritems():
                val.write(key)
                f.write('#include "%s"\n' % key)

            f.write('\n')
            write_dicts(f, self._defaults)
            write_dicts(f, self._sections)
        except:
            try:
                with open(f, 'wt') as fp:
                    self.write(fp)
            except IOError:
                print "Could not open file ", f, " for writing"