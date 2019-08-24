///////////////////////////////////////////////////////////////////
//
//      COPYRIGHT (C) SunilS GitHub, 2019
//      ALL RIGHTS RESERVED. UNPUBLISHED RIGHTS RESERVED
//      UNDER THE COPYRIGHT LAWS OF THE UNITED STATES.
//
//      THE SOFTWARE CONTAINED ON THIS MEDIA IS OPEN SOURCE
//      YOU MAY USE, DISTRIBUTE AND MODIFY THIS CODE UNDER THE
//      TERMS OF THE GPL LICENSE.
//      
//      NOTE: THE BELLOW WRITTEN CODE IS THE OUTPUT OF MY 
//		UNDERSTANDING AND CAN BE SOLELY USED FOR THE PURPOSE OF
//		GETTING INSIGHT ABOUT OPEN SOURCE API/TECHNOLOGIES. 
//
///////////////////////////////////////////////////////////////////

#define _GNU_SOURCE
#include <linux/module.h> 
#include <linux/init.h>   
#include <linux/kernel.h> 
#include <linux/miscdevice.h> 
#include <linux/version.h> 
#include <linux/kthread.h>    
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/preempt.h>
#include <linux/slab.h>


typedef struct EmployDB {
    char empName[80];
    uint64_t empId;
    struct list_head node;
    struct rcu_head rcu;
}EmployDB_t;

static LIST_HEAD(EmployDBHeadNode);
static spinlock_t writeSpinLock;
static int empNum = 0;

/** Prototype declaration*/
static void addEmploy(char *pEmpName);
static void freeCallback(struct rcu_head *rcu);
static void deleteEmployNode(char *pEmpName, int syncFree);
static void displayEmployDB();
static void updateEmployInfo(char *pEmpName, char *pEmpNewName, int syncFree);

static struct miscdevice misc_struct={
    //Minor number is given, if we give it should be unique and not taken by any module
    .minor = MISC_DYNAMIC_MINOR,
    .name = "Trial_Driver",
};

static void addEmploy(char *pEmpName)
{
    EmployDB_t *newEmploy ;

    newEmploy = kzalloc(sizeof(EmployDB_t), GFP_KERNEL);
    if(!newEmploy)
    {
        return;
    }

    strcpy(newEmploy->empName, pEmpName);
    newEmploy->empId = empNum++;

    spin_lock(&writeSpinLock);
    list_add_rcu(&newEmploy->node, &EmployDBHeadNode);
    spin_unlock(&writeSpinLock);
}


//Callback function for async reclaim
static void freeCallback(struct rcu_head *rcu)
{
    EmployDB_t *empNode  = container_of(rcu, struct  EmployDB, rcu);
    kfree(empNode);	
}

static void deleteEmployNode(char *pEmpName, int syncFree)
{
    EmployDB_t *empNode = NULL;
    
    spin_lock(&writeSpinLock);
    list_for_each_entry(empNode, &EmployDBHeadNode, node) {

        if(!strcmp(pEmpName, empNode->empName))
        {
            list_del_rcu(&empNode->node);
            spin_unlock(&writeSpinLock);

            if(syncFree) {
               printk("Calling synchronize_rcu.....\n");
                /** wait for all RCU read-side critical sections to complete*/
                synchronize_rcu();
                
                kfree(empNode); 			
            }
            else
            {
            	call_rcu(&empNode->rcu, freeCallback);  
            }
           return;
        }
    }

    spin_unlock(&writeSpinLock);
}

static void displayEmployDB()
{
    EmployDB_t *empNode = NULL;

    rcu_read_lock();

    list_for_each_entry_rcu(empNode, &EmployDBHeadNode, node) {
        printk("### Employ Name: {%s}\n", empNode->empName);
        pr_info("###### Employ ID: {%d}\n", empNode->empId);
    }
    rcu_read_unlock();
}

/* Update Employ details in the List*/
static void updateEmployInfo(char *pEmpName, char *pEmpNewName, int syncFree)
{
    EmployDB_t *empNode = NULL;
    EmployDB_t *empOldNode = NULL;
    EmployDB_t *empNewNode = NULL;
   
   
    rcu_read_lock();
    list_for_each_entry(empNode, &EmployDBHeadNode, node) {

        if(!strcmp(empNode->empName, pEmpName))
        {
            empOldNode = empNode;
            break;
        }
    }

    if(!empOldNode)
        return;

    empNewNode = kzalloc(sizeof(EmployDB_t), GFP_ATOMIC);
    if(!empNewNode)
    {
        return;
    }


    memcpy(empNewNode, empOldNode, sizeof(EmployDB_t));
    strcpy(empNewNode->empName, pEmpNewName);
    spin_lock(&writeSpinLock);
    list_replace_rcu(&empOldNode->node, &empNewNode->node );
    spin_unlock(&writeSpinLock);
    rcu_read_unlock();


    if(syncFree) {
        synchronize_rcu();
        kfree(empOldNode);		
    }
    else
    { 
        call_rcu(&empOldNode->rcu, freeCallback);       
    }
}


/*Initialization function */
static int __init trialInit(void)
{
    int result = 0;

    printk(KERN_INFO "In function intit module inserted\n");

    //0 - Success, -ve  on failure
    result = misc_register(&misc_struct);
    if(result < 0) 
        printk(KERN_ALERT "misc_register Failed..?");
    else
        printk(KERN_INFO " device Registered with minor number = %i \n", misc_struct.minor);

    //initialize spin lock
    spin_lock_init(&writeSpinLock);

    printk("\nAdding Employee's to DataBase:\n");
    //Initialize for Trident array  
    addEmploy("Employ1");
    addEmploy("Employ2");
    addEmploy("Employ3");
    addEmploy("Employ4");

    printk("Displaying Employ list:\n");
    //Display List
    displayEmployDB();

    printk("Deleting Employ from DB:\n");
    //Delete a Employ node
    deleteEmployNode("Employ3", 1);

    printk("Displaying Employ list:\n");
     //Display List
    displayEmployDB();

    printk("Updating Employ Name:\n");
    //Update a employ name
    updateEmployInfo("Employ1", "Employ3", 1);

    printk("Displaying Employ list:\n");
     //Display List
    displayEmployDB();
    
    return result;	
}

static void __exit trialExit(void)
{
    misc_deregister(&misc_struct);
    printk("\nDevice Unregistered\n");
}

module_init(trialInit);
module_exit(trialExit);

MODULE_AUTHOR("linusgeek.blogspot.com");
MODULE_DESCRIPTION("Trial driver for RCU");
MODULE_LICENSE("GPL");
