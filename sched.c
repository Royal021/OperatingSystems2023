#include "sched.h"

static volatile int can_schedule=0;
//we have to wrap the aligned variable in a structure
//(compiler requirement)
struct PageTable{
    unsigned table[4096] __attribute__((aligned(16384)));
};


struct PCB {
    //more about 'state' in a moment
    enum ProcessState state;
    u32 regs[16];
    u32 cpsr;
    struct PageTable* page_table;
};

enum ProcessState {
    VACANT, READY, RUNNING, SLEEPING, STARTING
};

#define MAXPROC 16
struct PCB process_table[MAXPROC];
struct PageTable page_tables[MAXPROC];
int current_pid = -1;


void sched_init()
{
    for (int i = 0; i < MAXPROC; i++){
        if (i == 0)
            process_table[i].state = STARTING;
        else
        {
            process_table[i].state = VACANT;
            //process_table[i].page_table = &page_tables[i];
        }
    }
    //Set all state fields to VACANT
    //Except process_table[0]...set that to STARTING
}

int spawn(const char* path)
{
    int pid = find_free_process_table_entry();
    if( pid == -1 )
        return EAGAIN;   //try again later
    clear_process_memory(pid);
    unsigned entryPoint;
    int rv = exec(path, (void*)(pid*0x400000), &entryPoint);  //load executable
    if(rv<0)
        return rv;
    initialize_process_page_table(pid);
    initialize_process_pcb(pid,...);
    return pid;
}

int find_free_process_table_entry()
{
    for (int i = 0; i < MAXPROC; i++){
        if (process_table[i].state == VACANT)
            return i;
    }
    return -1;
}


clear_process_memory(int pid)
{

    unsigned addr = (unsigned)pid*0x400000;
    kmemset( (char*)addr, 0, 4*1024*1024 );
    //clear the memory of the process
    //by clearing the page table entries
    //and then invalidating the TLB
    //for each page table entry
    //for (int i = 0; i < 4096; i++){
    //    page_tables[pid].table[i] = 0;
    //}
    //invalidate_tlb();
}

int initialize_process_page_table()
{
    unsigned* page_table = page_tables[pid].table;
    //use initialize process page table prob in exec
    /* do a for loop for this part or do it in the hard sets in itialize page table
    n = Pid * 4 + 0
    V = & 0xfffff
    v = v|(n<<12)
    Entry 4: Top bits = pid*4

    Entry 5: Top bits = pid*4+1

    Entry 6: Top bits = pid*4+2

    Entry 7: Top bits = pid*4+3 */
}


int initialize_process_pcb()
{
    struct PCB* pcb = &process_table[pid];

//r0...r12
for(int i=0;i<=12;++i)
    pcb->regs[i]=0;
pcb->regs[13] = EXE_STACK; //defined in exec lecture
pcb->regs[14] = 0;      //link register
pcb->regs[15] = entryPoint;     //pc

pcb->cpsr = (0x10 | (1<<6));    //next slide
pcb->page_table = page_tables[pid].table;

pcb->state = READY;
}

void schedule(unsigned registers[]){
    if(!can_schedule)
        return;
    int new_pid = sched_select_process();
    if( new_pid == current_pid )
        return;
    sched_save_process_status(current_pid,registers,READY);
    sched_restore_process_state(new_pid,registers,RUNNING);
}


int sched_select_process()
{
    int pid = -1;
    for (int i = 0; i < MAXPROC; i++){
        if (process_table[i].state == READY){
            pid = i;
            break;
        }
    }
    return pid;
}

void sched_save_process_status(int pid, unsigned registers[], enum ProcessState newState)
{
    if(current_pid == -1 )
        return;         //nothing to save
    struct PCB* pcb = &process_table[pid]
    pcb->state = newState;
    ...save pcb registers r0-r12 from registers[0...12]...
    pcb->regs[15] = registers[13];  //lr = old pc
    ...

    unsigned modeOfSuspendedTask = get_spsr() & 0x1f;
    unsigned modeForFetchingSpLr = 0;
    switch(modeOfSuspendedTask){
        case USR:
            modeForFetchingSpLr = SYSTEM;
            break;
        default:
            modeForFetchingSpLr = modeOfSuspendedTask;
            break;
    }
}


unsigned get_spsr()
{
    unsigned spsr;
    __asm__ volatile (
        "mrs %[spsrReg], spsr"
        : [spsrReg] "=r"(spsr)
    );
    return spsr;
}
unsigned get_cpsr()
{
    unsigned cpsr;
    __asm__ volatile (
        "mrs %[cpsrReg], cpsr"
        : [cpsrReg] "=r"(cpsr)
    );
    return cpsr ;
}

__asm__ volatile (
    //get cpsr
    "mrs r0, cpsr                   \n"
    //make a copy of it
    "mov r1, r0                     \n"
    //set low 5 bits
    "and r0, #0xffffffe0            \n"
    "orr r0, %[newMode]             \n"
    //set mode
    "msr cpsr, r0                   \n"
    //get banked sp and lr
    "mov %[oldSp], sp               \n"
    "mov %[oldLr], lr               \n"
    //restore mode
    "msr cpsr, r1                   \n"
    //get old cpsr
    "mrs %[oldCpsr], spsr           \n"

    :   [oldSp] "=&r"(pcb->regs[13]),
        [oldLr] "=&r"(pcb->regs[14]),
        [oldCpsr] "=&r"(pcb->cpsr)
    : [newMode] "r"(modeForFetchingSpLr)
    : "r0","r1","lr"
);


void sched_restore_process_state(int pid,
        unsigned* registers, enum ProcessState newState)
{
    if(current_pid >= MAXPROC)
        panic("Bad PID");
    struct PCB* pcb = &process_table[pid];
    ...copy register values r0-r12 from pcb to registers[0...12]...
    ...copy register r15 (pc) from pcb to registers[13]...
    unsigned modeForRestoredTask = process_table[pid].cpsr & 0x1f;
    unsigned modeForSpLr=0;

    switch(modeForRestoredTask)
    {
        case USR:
            modeForSpLr = SYSTEM;
            break;
        default:
            modeForSpLr = modeForRestoredTask;
            break;
    }
    __asm__ volatile (
    //get cpsr
    "mrs r0, cpsr               \n"
    //make a copy of it
    "mov r1, r0                 \n"
    //set low 5 bits
    "and r0, #0xffffffe0        \n"
    "orr r0, %[newMode]         \n"
    //set mode
    "msr cpsr, r0               \n"
    //set banked sp and lr
    "mov sp, %[oldSp]           \n"
    "mov lr, %[oldLr]           \n"
    //restore mode
    "msr cpsr, r1               \n"
    //set spsr
    "msr spsr, %[oldCpsr]       \n"
    :   //no outputs
    :   [oldSp] "r"(pcb->regs[13]),
        [oldLr] "r"(pcb->regs[14]),
        [oldCpsr] "r"(pcb->cpsr),
        [newMode] "r"(modeForSpLr)
    : "r0", "r1", "lr"
);
}

void sched_enable(){
    can_schedule=1;
}