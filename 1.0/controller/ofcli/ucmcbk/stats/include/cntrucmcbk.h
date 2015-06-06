#ifndef CNTRLR_UCMCBK_H
#define CNTRLR_UCMCBK_H
/* 
 * It describes the ucm component at Controller side
 */
int32_t of_switch_appl_ucmcbk_init (void);
int32_t of_domain_appl_ucmcbk_init (void);

int32_t of_switch_tablestats_ucmcbk_init (void);
int32_t of_switch_portstats_ucmcbk_init (void);
int32_t of_switch_flowstats_ucmcbk_init (void);

int32_t dprm_table_appl_ucmcbk_init (void);
int32_t datapath_appl_ucmcbk_init (void);
int32_t of_port_appl_ucmcbk_init (void);

int32_t of_portstats_appl_ucmcbk_init (void);
int32_t of_tablestats_appl_ucmcbk_init (void);
int32_t of_flowstats_appl_ucmcbk_init (void);
int32_t of_aggrstats_appl_ucmcbk_init (void);
int32_t of_tblfeatures_appl_ucmcbk_init(void);


#endif /*CNTRLR_UCMCBK_H*/
