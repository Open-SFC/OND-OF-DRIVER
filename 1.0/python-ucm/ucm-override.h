#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "trincld.h"
//#include "cmtransgdf.h"
#include "cmincld.h"
#include <signal.h>
#include <setjmp.h>
#include "cmdefs.h"
#include "cmutil.h"
#include "dmgdef.h"
#include "cmtransgdf.h"
#include "jegdef.h"
#include "jegif.h"
#include "histedit.h"
#include "transgif.h"
#include "jewrap.h"


#define SUCCESS 0
#define FAILURE -1


#define UCMJE_CONFIGREQUEST_PORT 63008


static PyObject* add_record(PyObject* self, PyObject * args, PyObject *kwargs);
static PyObject* update_record(PyObject* self,PyObject * args, PyObject *kwargs);
static PyObject* delete_record(PyObject* self,PyObject * args, PyObject *kwargs);
static PyObject* get_exact_record(PyObject* self, PyObject * args, PyObject *kwargs);


static inline PyObject* send_cmd(uint32_t command_id,PyObject * args, PyObject *kwargs);

void *create_config_session(void);
void create_transport_channel(void);
void close_transport_channel(void);
