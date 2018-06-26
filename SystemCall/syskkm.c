#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/sched/signal.h>
#include <linux/thread_info.h>
#include <linux/init.h>
#include <linux/mm_types.h>
#include "./sched/sched.h"
MODULE_LICENSE("GPL");

asmlinkage long sys_kkmcall(int option, int pid){
	struct task_struct *task;
//	int check=0;
	char const state_array[10]={'R', 'S', 'D', 'T','t', 'Z', 'X'};
	printk("PS:\n");
	if(option==0){	//ps (sysps)

		printk("%s %5s %5s %5s %5s %5s %5s %s","PS: ",  "UID","PID", "PPID", "NICE", "STATE", "CPU#", "CMD" );	
		for_each_process(task){
//			check++;

		unsigned int state = (task->state | task->exit_state) & TASK_REPORT;
		char chstate=state_array[state];
		printk("PS:  %5u %5d %5d %5d %5c %5u %s", task->real_cred->uid, task->pid, task->real_parent->pid,task_nice(task),chstate , task_cpu(task), task->comm);
//			if(check==10)
//				return 0;
		
		}
		return 0;
	}
	else if(option==1){	//CPU_INFO (sysps -C)
		int cpu;
		printk("%s %5s %5s %10s", "PS: ","CPU#", "nr_switches", "nr_running" );
		for_each_possible_cpu(cpu){
//			check++;
			printk("%s %5d %11d %10u","PS: ", cpu,cpu_rq(cpu)->nr_switches, cpu_rq(cpu)->nr_running  );
//			if(check==10)
//				return 0;
		}
	}

	else if(option==2){	//SIBLING (sysps -s -p processid)	siblings of pid 
		printk("%s Print Sibling of %d","PS:", pid);
		printk("%s %5s %5s %5s %5s %5s %5s %s","PS: ",  "UID","PID", "PPID", "NICE", "STATE", "CPU#", "CMD" );
		int parent;
		struct task_struct *temp;
		for_each_process(task){
			if(task->pid==pid){
//				INIT_LIST_HEAD(&temp->sibling);
				parent=task->real_parent->pid;

				struct list_head *pos;
				list_for_each(pos,&task->sibling){
					temp=list_entry(pos, struct task_struct, sibling);
					if(parent==temp->pid || temp->pid==0) continue;
					unsigned int state = (temp->state | temp->exit_state) & TASK_REPORT;
					char chstate=state_array[state];
 					printk("PS:  %5u %5d %5d %5d %5c %5u %s", temp->real_cred->uid, temp->pid, parent,task_nice(temp),chstate, task_cpu(temp), temp->comm);
				}
				return 0;
			}
		}
	return 0;
	}
	else if(option==3){	//CHILDREN )sysps -c -p processid)  childrens of pid
	   printk("%s Print Children of %d","PS:", pid);
	   printk("%s %5s %5s %5s %5s %5s %5s %s","PS: ",  "UID","PID", "PPID", "NICE", "STATE", "CPU#", "CMD" );

		struct task_struct *temp;
		for_each_process(task){
			if(task->pid==pid){
		
			struct list_head *pos;
			list_for_each(pos,&task->children){
				if(pos==NULL)return 0;
				temp=list_entry(pos, struct task_struct, sibling);

				unsigned int state = (temp->state | temp->exit_state) & TASK_REPORT;
				char chstate=state_array[state];
 			printk("PS:  %5u %5d %5d %5d %5c %5u %s", temp->real_cred->uid, temp->pid, pid,task_nice(temp),chstate, task_cpu(temp), temp->comm);
		}
			return 0;
			}
		}
		
	return 0;
	}
	else if(option==4){	//VMA (sysps-v)
		struct mm_struct * mm;
		struct vm_area_struct * mmap;
		printk("%s %5s %29s %29s %29s %14s %7s\n", "PS: ","PID", "CODE", "DATA", "HEAP", "STACK", "#VMA");
		for_each_process(task){
			mm=task->mm;
			if(mm!=NULL){
				mmap=mm->mmap;	
				
				printk("%s %5d 0x%012x-0x%012x 0x%012x-0x%012x 0x%012x-0x%012x 0x%012x- %6d","PS: ", 
					task->pid,mm->start_code, mm->end_code, mm->start_data, mm->end_data, 
					mm->start_brk, mm->brk, mm->start_stack, mm->map_count);
				
			}
		}
	}
	else
		return 0;
	return 0;
}
