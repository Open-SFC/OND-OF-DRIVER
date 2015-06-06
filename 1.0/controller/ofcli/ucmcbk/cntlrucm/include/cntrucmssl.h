#ifndef __CNTRUCMSSL_H
#define __CNTRUCMSSL_H

#define CNTLR_CM_CERT_FILE_PATH_SIZE 256

struct cntlr_ucm_certs_info{
   char ca_cert_file[CNTLR_CM_CERT_FILE_PATH_SIZE];
   char self_cert_file[CNTLR_CM_CERT_FILE_PATH_SIZE];
   char priv_key_file[CNTLR_CM_CERT_FILE_PATH_SIZE];
   char priv_key_pwd[32];
};

struct cntlr_ucm_cacert_info{
   char ca_cert_file[CNTLR_CM_CERT_FILE_PATH_SIZE];
};

#endif
