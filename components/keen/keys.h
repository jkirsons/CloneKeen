// key defines, indexes in keytable[]
#define KEYTABLE_SIZE            32
#define KEYMAPPINGVERSION		 2		// for config file. increment if you change this file.

#define NOKEY		  0		// this is used internally by the keydrv
#define KESC          1
#define KLEFT         2
#define KRIGHT        3
#define KUP           4
#define KDOWN         5
#define KCTRL         6
#define KALT          7
#define KENTER        8
#define KSPACE        9
#define KF1           10
#define KF2           11
#define KF3           12
#define KF4           13
#define KF5           14
#define KF6           15
#define KF7           16
#define KF8           17
#define KF9           18
#define KF10          19

// controls for non-primary player
#define KLEFT2        20
#define KRIGHT2       21
#define KUP2          22
#define KDOWN2        23
#define KCTRL2        24
#define KALT2         25
#define KENTER2		  26	
#define KSPACE2		  27

#define KPLUS         28
#define KMINUS		  29
#define KDEL		  30
#define KTAB          31


// keycodes used in each player[x].keytable[] struct. these are compiled from
// the global keytable[] structure.
#define PKEYTABLE_SIZE	8

#define PKLEFT			0
#define PKRIGHT			1
#define PKUP			2
#define PKDOWN			3
#define PKSTATUS		4
#define PKJUMP         	5
#define PKPOGO         	6
#define PKFIRE         	7

