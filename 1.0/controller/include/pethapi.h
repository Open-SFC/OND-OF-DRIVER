#ifndef __PSEUDO_ETH_API_H_
#define __PSEUDO_ETH_API_H_

enum peth_app_ids {
        PETH_APPID_FORWARDING = 0,
        PETH_APPID_IPSEC,
        PETH_APPID_MAX
};

/* Prototype of callback function for application to receive packets from
 * peth threads.
 */
typedef int (*peth_app_pkt_rcv_fn) (struct pkt_mbuf *pkt_ptr, uint32_t nsid,
				int if_index, uint32_t peth_thread_id);

int peth_app_reg_pkt_rcv_function (uint8_t app_id, peth_app_pkt_rcv_fn rcv_fn);

int peth_app_dereg_pkt_rcv_function (uint8_t app_id);

#define OF_PETH_MAX_MSG_QUEUE_LIST  64


void peth_app_init (void);
void peth_app_deinit (void);

int peth_create_net_dev_iface (char *ns_name, uint32_t nsid, char *peth_ifname,
                                int32_t *if_index_p);

int peth_delete_net_dev_iface (char *ns_name, uint32_t nsid, int32_t if_index);

int peth_app_send_pkt_to_kernel (uint32_t nsid, uint32_t ifid, char *ifname,
                struct pkt_mbuf *pkt_ptr);

int peth_trigger_arp_request (char *ns_name, int32_t if_index,
                                uint32_t target_ip);

struct peth_queue_elem {
	void		*kern_va_p;
	void		*data_kva_p;
	unsigned long	kern_to_user_offset;
};

/*
 * Structrue of a queue between user space application and pEth net device.
 * It is a circular queue with read and write indices wrapping around.
 * The queue is full when write == read.
 * Writing should never overwrite the read position
 */
struct peth_net_queue {
        volatile uint16_t	write; /* Next position to be written*/
        volatile uint16_t	read; /* Last read position */
        uint16_t		queue_size; /* Max elements in the queue */
        uint16_t		elem_size; /* size of each queue element */

	/* arrary of struct peth_queue_elem */
        struct peth_queue_elem volatile	queue_elems[0];
};

uint16_t peth_drv_queue_count (struct peth_net_queue *queue_p);

uint16_t peth_drv_queue_free_count (struct peth_net_queue *queue_p);

uint16_t peth_drv_queue_get (struct peth_net_queue *queue_p,
			struct peth_queue_elem *data, uint16_t num_elem);

uint16_t peth_drv_queue_put (struct peth_net_queue *queue_p,
                        struct peth_queue_elem *data, uint16_t num_elem);

void peth_net_queue_init (struct peth_net_queue *queue_p, uint16_t size);


struct peth_char_dev_info {
        int32_t         fd;
//      char            dev_name[PETH_CHAR_DRV_INST_NAME_LEN_MAX+1];
//      of_list_t       net_dev_list;
        struct mchash_table     *net_dev_table;
};

#define PETH_NET_DEV_NAME_LEN_MAX	(15)

/*
 * Structure used to create a pEth net device. Passed to the kernel in IOCTL call
 */
struct peth_net_dev_info {
        char		dev_name[PETH_NET_DEV_NAME_LEN_MAX+1];

        void		**tx_q_p;
        void		**rx_q_p;
        void		**alloc_q_p;
        void		**free_q_p;

	void		*shmem_kern_ptr;

	int		dev_if_index;
};

struct peth_arp_trigger_ioctl_param {
	int32_t if_index;
	uint32_t target_ip;
};

struct peth_thread_kern_info {
	uint8_t		num_peth_kern_threads;
	uint8_t		peth_starting_core_id;
	uint8_t		peth_num_cores;
};

enum peth_ioctl_cmd_nr{
	PETH_IOCTL_CMD_NR_CREATE_NET_DEV = 1,
	PETH_IOCTL_CMD_NR_DELETE_NET_DEV,
	PETH_IOCTL_CMD_NR_TRIGGER_ARP_REQ,
	PETH_IOCTL_CMD_NR_MAP_KERN_MEMORY,
	PETH_IOCTL_CMD_NR_UNMAP_KERN_MEMORY,
	PETH_IOCTL_CMD_NR_CREATE_KERN_THREADS,
	PETH_IOCTL_CMD_NR_STOP_KERN_THREADS,
	PETH_IOCTL_CMD_NR_IPSEC_PLUGIN
};


#define PETH_IOCTL_CREATE_NET_DEVICE    _IOWR(0, PETH_IOCTL_CMD_NR_CREATE_NET_DEV, struct peth_net_dev_info)
#define PETH_IOCTL_DELETE_NET_DEVICE    _IOWR(0, PETH_IOCTL_CMD_NR_DELETE_NET_DEV, int)
#define PETH_IOCTL_TRIGGER_ARP_REQ	_IOWR(0, PETH_IOCTL_CMD_NR_TRIGGER_ARP_REQ, struct peth_arp_trigger_ioctl_param)
#define PETH_IOCTL_MAP_KERN_MEMORY	_IOWR(0, PETH_IOCTL_CMD_NR_MAP_KERN_MEMORY, struct mem_region_info)
#define PETH_IOCTL_UNMAP_KERN_MEMORY	_IOWR(0, PETH_IOCTL_CMD_NR_UNMAP_KERN_MEMORY, struct mem_region_info)

#define PETH_IOCTL_CREATE_KERN_THREADS	_IOWR(0, PETH_IOCTL_CMD_NR_CREATE_KERN_THREADS, struct peth_thread_kern_info)

#define PETH_IOCTL_STOP_KERN_THREADS	_IOWR(0, PETH_IOCTL_CMD_NR_STOP_KERN_THREADS, int)

#define OF_IPSEC_IOCTL_HANDLER		_IOWR(0, PETH_IOCTL_CMD_NR_IPSEC_PLUGIN, struct of_ipsec_ioctl_info)

#define PSEUDO_ETH_CHAR_DEV_MAJOR_NUM 130
#define PETH_CHAR_DRIVER	"/dev/pethchar"

#undef pr_debug
#undef pr_err

#define PETH_APP_ERROR_PRINT

#ifdef PETH_APP_ERROR_PRINT
#define pr_err printk

#else
#define pr_err(format,args...)
#endif

//#define PETH_APP_DEBUG_PRINT

#ifdef PETH_APP_DEBUG_PRINT
#define pr_debug printk

#else
#define pr_debug(format,args...)
#endif

#endif /* __PSEUDO_ETH_API_H_ */
