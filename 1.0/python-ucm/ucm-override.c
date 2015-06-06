#include <Python.h>
#include "ucm-override.h"

/***************************************************************************************/
/********************************  Global Variables ************************************/
/***************************************************************************************/
//#define INFRA_DEBUG
#ifdef INFRA_DEBUG
#define INFRA_PRINT(format,args...) do{ printf("%s:%s:%d "format,__FILE__,__FUNCTION__,__LINE__,##args);}while(0)
#else
#define INFRA_PRINT(format,args...)
#endif /* INFRA DEBUG */
static PyObject *UCMException;
static void *transport_channel_g=NULL;



static PyMethodDef ucmext_methods[] = {
    {"add_record", (PyCFunction)add_record, METH_VARARGS | METH_KEYWORDS, "Add a Record"},
    {"update_record", (PyCFunction)update_record, METH_VARARGS | METH_KEYWORDS, "Update a Record"},
    {"delete_record", (PyCFunction)delete_record, METH_VARARGS | METH_KEYWORDS, "Delete a Record"},
    {"get_exact_record", (PyCFunction)get_exact_record, METH_VARARGS | METH_KEYWORDS, "Get Record with given key parameter"},
    {NULL,NULL,0,NULL}
};




static PyObject* get_exact_record(PyObject* self, PyObject *args, PyObject *kwargs)
{
    PyObject* dict=NULL,*dmpath=NULL, *ip=NULL , *port=NULL;
    PyObject *name=NULL, *value=NULL, *value_type=NULL, *value_data=NULL;
    PyObject *pResultDict = PyDict_New(); 
    Py_ssize_t pos = 0;
    static char *kwlist[] = {"dict", "ip", "port", NULL}, *dmpath_p;
    struct cm_nv_pair *nv_pair;
    int32_t ret;
    uint32_t ui=0;
    struct cm_nv_pair *nv_pairs;
    struct cm_array_of_nv_pair *out_nv_pair_array=NULL,in_nv_pair_array;


    PyArg_ParseTupleAndKeywords(args,kwargs,"O!|si", kwlist,&PyDict_Type, &dict, &ip, &port);
    PyErr_Clear();

    // get data model path or dmpath from the dictionary
    dmpath = PyDict_GetItemString(dict,"dmpath");
    if (dmpath)
        PyDict_DelItemString(dict,"dmpath");
    else
    {
        PyErr_SetString(UCMException,"Could not find dmpath in the dictionary.");
        Py_RETURN_NONE;
    }
    INFRA_PRINT("DMPath = %s************************\n",PyString_AsString(dmpath));

    // Check Transport Channel
    if(! transport_channel_g)
    {
        create_transport_channel();
        if(! transport_channel_g)
        {
            Py_RETURN_NONE;
        }
    }

    // Update command structure with command, dmpath, and number of parameters
    dmpath_p = PyString_AsString(dmpath);
    in_nv_pair_array.count_ui = PyDict_Size(dict);

    // allocate memory for the parameters
    nv_pairs = (struct cm_nv_pair *) calloc(in_nv_pair_array.count_ui,sizeof(struct cm_nv_pair));

    // update command structure with paramters pointer
    in_nv_pair_array.nv_pairs = nv_pairs;

    // Populate name value pairs
    while(PyDict_Next(dict, &pos, &name, &value)){
        if(PyDict_CheckExact(value)){
            value_type = PyDict_GetItemString(value,"type");
            value_data = PyDict_GetItemString(value,"value");
            nv_pairs->value_type = (uint32_t)PyInt_AsUnsignedLongMask(value_type);
            nv_pairs->name_p = PyString_AsString(name);
            nv_pairs->name_length=strlen(nv_pairs->name_p);
            nv_pairs->value_p = PyString_AsString(value_data);
            nv_pairs->value_length = strlen(nv_pairs->value_p);
            INFRA_PRINT("Key = %s**************\ntype = %u, value = %s***************\n\n",nv_pairs->name_p, nv_pairs->value_type, (char *) nv_pairs->value_p);
            nv_pairs++;
        }
    }

    ret = cm_get_exact_record(transport_channel_g,CM_REST_MGMT_ENGINE_ID,"UCMRole",dmpath_p, &in_nv_pair_array, &out_nv_pair_array);

    //free allocated nv pairs
    free(in_nv_pair_array.nv_pairs);
    INFRA_PRINT("return value = %d\n\n",ret);

    if(ret == SUCCESS)
    {
        INFRA_PRINT("NV Pair Array pointer = %p\n",out_nv_pair_array);
        nv_pair = out_nv_pair_array->nv_pairs;

        for(ui=0;ui<out_nv_pair_array->count_ui;ui++)
        {
            PyDict_SetItemString(pResultDict, nv_pair->name_p,Py_BuildValue("s", nv_pair->value_p));
            INFRA_PRINT("NV Pair pointer = %p\n",nv_pair);
            INFRA_PRINT("key = %s ======== value %s\n",nv_pair->name_p,(char *)nv_pair->value_p);
            nv_pair++;
        }
        CM_FREE_PTR_NVPAIR_ARRAY(out_nv_pair_array,out_nv_pair_array->count_ui);
    } else
    {
        INFRA_PRINT("Cm get record failed.\n");
        PyErr_SetString(UCMException,"Unable to get record");
        Py_XDECREF(pResultDict);
        close_transport_channel();
        Py_RETURN_NONE;
    }
    return pResultDict;
}


static PyObject* add_record(PyObject* self,PyObject * args, PyObject *kwargs)
{
    PyErr_Clear();
    return send_cmd(CM_CMD_ADD_PARAMS,args,kwargs);
}


static PyObject* update_record(PyObject* self,PyObject * args, PyObject *kwargs)
{
    PyErr_Clear();
    return send_cmd(CM_CMD_SET_PARAMS, args, kwargs);
}


static PyObject* delete_record(PyObject* self,PyObject * args, PyObject *kwargs)
{
    PyErr_Clear();
    return send_cmd(CM_CMD_DEL_PARAMS, args, kwargs);
}



static inline PyObject* send_cmd(uint32_t command_id,PyObject * args, PyObject *kwargs)
{
    void *config_session_p=NULL;
    struct cm_result *result_p=NULL;
    PyObject* dict=NULL,*dmpath=NULL, *ip=NULL , *port=NULL;
    PyObject *name=NULL, *value=NULL, *value_type=NULL, *value_data=NULL;
    int32_t ret;
    Py_ssize_t pos = 0;
    static char *kwlist[] = {"dict", "ip", "port", NULL};
    struct cm_command command_info={0};
    struct cm_nv_pair *nv_pairs;

    // Parse the Configuration Parameters
    // FIXME: Need to handle the IP Address and port parameter to connect particular JE.
    //        Issue to think is about the "JE, lxos, misc, transport channel" shared objects 
    //        required by this python ucm module.
    PyArg_ParseTupleAndKeywords(args,kwargs,"O!|si", kwlist,&PyDict_Type, &dict, &ip, &port);

    // get data model path or dmpath from the dictionary
    dmpath = PyDict_GetItemString(dict,"dmpath");
    if (dmpath)
        PyDict_DelItemString(dict,"dmpath");
    else
    {
        PyErr_SetString(UCMException,"Could not find dmpath in the dictionary.");
        Py_RETURN_NONE;
    }
    INFRA_PRINT("DMPath = %s************************\n",PyString_AsString(dmpath));

    // Check Transport Channel
    if(! transport_channel_g)
    {
        create_transport_channel();
        if(! transport_channel_g)
        {
            Py_RETURN_NONE;
        }
    }

    // Creating Configuration Session
    config_session_p = create_config_session();
    if(! config_session_p)
    {
        // Some Error occured and config session is not created. close transport channel and return None
        close_transport_channel();
        Py_RETURN_NONE;
    }

    // Update command structure with command, dmpath, and number of parameters
    command_info.command_id = command_id;
    command_info.dm_path_p = PyString_AsString(dmpath);
    command_info.nv_pair_array.count_ui = PyDict_Size(dict);

    // allocate memory for the parameters
    nv_pairs = (struct cm_nv_pair *) calloc(command_info.nv_pair_array.count_ui,sizeof(struct cm_nv_pair));

    // update command structure with paramters pointer
    command_info.nv_pair_array.nv_pairs = nv_pairs;

    // Populate name value pairs
    while(PyDict_Next(dict, &pos, &name, &value)){
        if(PyDict_CheckExact(value)){
            value_type = PyDict_GetItemString(value,"type");
            value_data = PyDict_GetItemString(value,"value");
            nv_pairs->value_type = (uint32_t)PyInt_AsUnsignedLongMask(value_type);
            nv_pairs->name_p = PyString_AsString(name);
            nv_pairs->name_length=strlen(nv_pairs->name_p);
            nv_pairs->value_p = PyString_AsString(value_data);
            nv_pairs->value_length = strlen(nv_pairs->value_p);
            INFRA_PRINT("Key = %s**************type = %u, value = %s***************\n\n",nv_pairs->name_p, nv_pairs->value_type, (char *) nv_pairs->value_p);
            nv_pairs++;
        }
    }

    // Sending command to UCM
    ret = cm_config_session_update_cmd(config_session_p,&command_info,&result_p);

    //free allocated nv pairs
    free(command_info.nv_pair_array.nv_pairs);
    INFRA_PRINT("return value = %d\n\n",ret);
    
    if(ret != SUCCESS)
    {
        INFRA_PRINT("Command execution failed.\n");
        if(result_p)
        {
            INFRA_PRINT("result pointer = %p\n",result_p);
            INFRA_PRINT("result pointer = %p\n",result_p->result.error.result_string_p);
            INFRA_PRINT(" Error Message = %s\n\n",(result_p->result.error.result_string_p));
            if((result_p->result).error.result_string_p)
                PyErr_SetString(UCMException,(result_p->result).error.result_string_p);
            else
                PyErr_SetString(UCMException,"Unable to configure the record.");
            UCMFreeUCMResult (result_p);
        }
        cm_config_session_end(config_session_p,CM_CMD_CONFIG_SESSION_REVOKE,&result_p);
        if(result_p)
            UCMFreeUCMResult(result_p);
        close_transport_channel();
        Py_RETURN_NONE;
    }else
    {
        INFRA_PRINT("Command execution succeeded.\n");
        if(result_p)
        {
            INFRA_PRINT("In Result. Result Pointer = %p\n",result_p);
            UCMFreeUCMResult(result_p);
        }

        result_p = NULL;

        ret = cm_config_session_end(config_session_p,CM_CMD_CONFIG_SESSION_COMMIT,&result_p);
        INFRA_PRINT("Sent Config Commit command.\n");
        if(ret != SUCCESS)
        {
            INFRA_PRINT("Command execution failed.\n");
            if(result_p)
            {
                INFRA_PRINT("result pointer = %p\n",result_p);
                INFRA_PRINT("result error pointer = %p\n",&(result_p->result).error);
                INFRA_PRINT("result String pointer = %p\n",(result_p->result).error.result_string_p);
                INFRA_PRINT(" Error Message = %s\n\n",(result_p->result).error.result_string_p);
                if((result_p->result).error.result_string_p)
                {
                    PyErr_SetString(UCMException,(result_p->result).error.result_string_p);
                    INFRA_PRINT("Error string is set as %s\n",(result_p->result).error.result_string_p);
                }else
                    PyErr_SetString(UCMException,"Unable to configure the record.");
                UCMFreeUCMResult (result_p);
            }
            INFRA_PRINT("Revoking the session.\n");
            result_p = NULL;
            cm_config_session_end(config_session_p,CM_CMD_CONFIG_SESSION_REVOKE,&result_p);
            if(result_p)
                UCMFreeUCMResult(result_p);
            close_transport_channel();
            Py_RETURN_NONE;
        }
    }
    return Py_BuildValue("i",SUCCESS);
}


void create_transport_channel(void)
{
    transport_channel_g =  cm_tnsprt_create_channel (CM_IPPROTO_TCP,0, 0, UCMJE_CONFIGREQUEST_PORT);
    if(transport_channel_g == NULL)
    {
        INFRA_PRINT("Unable to create Transport Channel.\n");
        PyErr_SetString(UCMException, "Unable to create Transport Channel");
    }
    INFRA_PRINT("Transport Channel created...\n");
}


void close_transport_channel(void)
{
    INFRA_PRINT("Closing the Transport Channel...\n");
    cm_tnsprt_close_channel((struct cm_tnsprt_channel *)transport_channel_g);
    transport_channel_g = NULL;
    INFRA_PRINT("Transport Channel closed...\n");
}



void *create_config_session(void)
{
    struct cm_role_info role_info={.admin_name="UCM",.admin_role="UCMRole"};
    void * config_session_p=NULL;

    config_session_p = cm_config_session_start(CM_REST_MGMT_ENGINE_ID,&role_info,transport_channel_g);
    if(config_session_p == NULL)
    {
        INFRA_PRINT("Unable to create Config Session.\n\n");
        PyErr_SetString(UCMException,"Unable to create Config Session");
        return NULL;
    }
    INFRA_PRINT("Config Session created.\n");
    return config_session_p;
}


void init_ucm(void)
{
    PyObject *m;

    m = Py_InitModule("_ucm", ucmext_methods);
    if(m == NULL)
        return;

    UCMException = PyErr_NewException("_ucm.UCMException", NULL, NULL);
    Py_XINCREF(UCMException);
    PyModule_AddObject(m,"UCMException",UCMException);
}
