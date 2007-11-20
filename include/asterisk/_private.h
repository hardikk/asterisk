/*
 * Prototypes for public functions only of internal interest,
 * normally not used by modules.
 * What goes here are typically *_init() routines.
 */

#ifndef _ASTERISK__PRIVATE_H
#define _ASTERISK__PRIVATE_H

int load_modules(unsigned int);		/*!< Provided by loader.c */
int load_pbx(void);			/*!< Provided by pbx.c */
int init_logger(void);			/*!< Provided by logger.c */
void close_logger(void);		/*!< Provided by logger.c */
int init_framer(void);			/*!< Provided by frame.c */
int ast_term_init(void);		/*!< Provided by term.c */
int astdb_init(void);			/*!< Provided by db.c */
void ast_channels_init(void);		/*!< Provided by channel.c */
void ast_builtins_init(void);		/*!< Provided by cli.c */
int dnsmgr_init(void);			/*!< Provided by dnsmgr.c */ 
void dnsmgr_start_refresh(void);	/*!< Provided by dnsmgr.c */
int dnsmgr_reload(void);		/*!< Provided by dnsmgr.c */
void threadstorage_init(void);		/*!< Provided by threadstorage.c */
void ast_event_init(void);		/*!< Provided by event.c */
int ast_device_state_engine_init(void);	/*!< Provided by devicestate.c */
int astobj2_init(void);			/*! Provided by astobj2.c */

/*!
 * \brief Reload asterisk modules.
 * \param name the name of the module to reload
 *
 * This function reloads the specified module, or if no modules are specified,
 * it will reload all loaded modules.
 *
 * \note Modules are reloaded using their reload() functions, not unloading
 * them and loading them again.
 * 
 * \return 0 if the specified module was not found.
 * \retval 1 if the module was found but cannot be reloaded.
 * \retval -1 if a reload operation is already in progress.
 * \retval 2 if the specfied module was found and reloaded.
 */
int ast_module_reload(const char *name);

#endif /* _ASTERISK__PRIVATE_H */
