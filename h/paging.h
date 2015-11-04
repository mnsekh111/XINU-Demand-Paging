/* paging.h */

#define NBPG		4096	/* number of bytes per page	*/
#define FRAME0		1024	/* zero-th frame		*/

#define BSM_UNMAPPED	0
#define BSM_MAPPED	1

#define FRM_UNMAPPED	0
#define FRM_MAPPED	1

#define FR_PAGE		0
#define FR_TBL		1
#define FR_DIR		2

#define FIFO		3
#define LRU		4

#define MAX_ID          15              /* You get 16 mappings, 0 - 15 */

#define BACKING_STORE_BASE	0x00800000
#define BACKING_STORE_UNIT_SIZE 0x00080000
#define NFRAMES		1024
#define NBS		16	/* Total number of backing stores */

typedef unsigned int bsd_t;

/* Structure for a page directory entry */

typedef struct {
    unsigned int pd_pres : 1; /* page table present?		*/
    unsigned int pd_write : 1; /* page is writable?		*/
    unsigned int pd_user : 1; /* is use level protection?	*/
    unsigned int pd_pwt : 1; /* write through cachine for pt?*/
    unsigned int pd_pcd : 1; /* cache disable for this pt?	*/
    unsigned int pd_acc : 1; /* page table was accessed?	*/
    unsigned int pd_mbz : 1; /* must be zero			*/
    unsigned int pd_fmb : 1; /* four MB pages?		*/
    unsigned int pd_global : 1; /* global (ignored)		*/
    unsigned int pd_avail : 3; /* for programmer's use		*/
    unsigned int pd_base : 20; /* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct {
    unsigned int pt_pres : 1; /* page is present?		*/
    unsigned int pt_write : 1; /* page is writable?		*/
    unsigned int pt_user : 1; /* is use level protection?	*/
    unsigned int pt_pwt : 1; /* write through for this page? */
    unsigned int pt_pcd : 1; /* cache disable for this page? */
    unsigned int pt_acc : 1; /* page was accessed?		*/
    unsigned int pt_dirty : 1; /* page was written?		*/
    unsigned int pt_mbz : 1; /* must be zero			*/
    unsigned int pt_global : 1; /* should be zero in 586	*/
    unsigned int pt_avail : 3; /* for programmer's use		*/
    unsigned int pt_base : 20; /* location of page?		*/
} pt_t;

typedef struct {
    unsigned int pg_offset : 12; /* page offset			*/
    unsigned int pt_offset : 10; /* page table offset		*/
    unsigned int pd_offset : 10; /* page directory offset	*/
} virt_addr_t;

struct bs_proc_map_t {
    int pid;
    unsigned int vpageno;
    struct bs_proc_map_t *next;
};

struct fifoqueue {
    int fn;
    struct fifoqueue *nextf;
};

typedef struct {
    int bs_status; /* MAPPED or UNMAPPED		*/
    struct bs_proc_map_t *mapping; /* mapping to all pids which use a backing store */
    int bs_npages; /* number of pages in the store */
    int bs_private; /* for vheap?	*/
} bs_map_t;

typedef struct {
    int fr_status; /* MAPPED or UNMAPPED		*/
    int fr_pid; /* process id using this frame  */
    int fr_curr_bs; /* current bs using this frame*/
    struct bs_map_t *fr_bs_list; /*to keep track of multiple bs using this frame */
    int fr_vpno[NPROC]; /* corresponding virtual page no*/
    int fr_curr_page; /* curr page in the frame */
    int fr_refcnt; /* reference count		*/
    int fr_type; /* FR_DIR, FR_TBL, FR_PAGE	*/
    int fr_dirty;
    void *cookie; /* private data structure	*/
    unsigned long int fr_loadtime; /* when the page is loaded 	*/
} fr_map_t;

struct fr_list {
    int fn;
    struct fr_list *next;
};



extern bs_map_t bsm_tab[NBS];
extern fr_map_t frm_tab[NFRAMES];

extern int next_frame();

/* Prototypes for required API calls */
SYSCALL xmmap(int, bsd_t, int);
SYSCALL xunmap(int);

/* given calls for dealing with backing store */

int get_bs(bsd_t, unsigned int);
SYSCALL release_bs(bsd_t);
SYSCALL read_bs(char *, bsd_t, int);
SYSCALL write_bs(char *, bsd_t, int);



