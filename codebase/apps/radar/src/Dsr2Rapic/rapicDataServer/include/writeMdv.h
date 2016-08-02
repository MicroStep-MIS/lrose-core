#ifndef _writeMdv_H
#define _writeMdv_H

#include "spinlock.h"
#include "rdrscan.h"
#include "cdata.h"
#ifdef STDCPPHEADERS
#include <list>
#include <vector>
#else
#include <list.h>
#include <vector.h>
#endif

#define RAPIC_DBZ_BYTES sizeof(unsigned char)
#define RAPIC_VEL_BYTES sizeof(unsigned char)
#define RAPIC_DBZ_SCALE 1 //2   //used to cram floats into 8 bits
#define RAPIC_DBZ_OFFSET 0 //32 //
#define RAPIC_VEL_SCALE 10 //2   //
#define RAPIC_VEL_OFFSET 0 //64 //use nyquist value in code

#define NO_GATES 200


class writeMdv : public scan_client {
    THREADID_T	 Thread_pid,Thread_ppid;
    int		 StopThreadFlag;
    int		 VolumeNo;  //count of volumes (scans in migfese)
    static void	 ThreadEntry(void *thiswriteMdv);
    spinlock	*lock;
    spinlock	*mdvDataListLock;
    void	 runLoop();
    void	 readInitFile(char *initfilename = 0);
    void         Init_mdv();
    int	         rapicToRadial(bool last_radial, rdr_scan *reflscan, s_radl
			       *refl_radl, rdr_scan *velscan, s_radl *vel_radl,
			       caddr_t *mdv_radial );
    rdr_scan_node    *newscans_last, *checkedscans_last;
    bool	 debug;
    e_scan_type  radarType;
    char         radarTypeStr[20];
    char	 path[255];
    
    
public:
    writeMdv();
    virtual	    ~writeMdv();
    void	    StartThread();
    void	    StopThread();
    virtual void    GetListLock();  // allow MdvDataNode List locking
    virtual void    RelListLock();  // MUST BE USED RESPONSIBLY!!!
    int		    mdvDataNodeCount;
    int		    mdvDataNodePos;	
    bool	    mdvFlag;
    int             station;
    char 	    stationName[100];
    float	    latit,longit,alt;

    rdr_scan_node   *MdvDataNodes;	// linked list of rdr nodes
    rdr_scan_node   *ThisMdvDataNode;	// linked list of incomplete rdr nodes



    virtual int	    NewDataAvail(rdr_scan *newscan); //return 1 if scan added, else 1
    virtual int	    NewDataAvail(CData *newscan); // new scan passed by ScanMng
    virtual int	    FinishedDataAvail(rdr_scan *finishedscan); // add finished scan to new sca list. usually called by other thread
    virtual int	    FinishedDataAvail(CData *newscan); // add finished scan to new sca list. usually called by other thread
// quickly check new scans list, add scans to keep to checkedscans, discard others
    virtual void    CheckNewScans(bool MoveToChecked = FALSE, 
			bool RejectFaulty = TRUE); // keep this quick, uses lock
/*
 * ProcessCheckedScans doesn't need to be locked,  can perform 
 * time consuming operations
 */				
    virtual void    ProcessCheckedScans(int maxscans = -1);// process checked scans list
};

extern writeMdv *MdvDataManager;

#endif //_writeMdv_H
