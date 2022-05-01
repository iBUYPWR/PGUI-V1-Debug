#ifndef NEWTYPE_H
#define NEWTYPE_H

#include <math.h>
#include <assert.h>

#include <QStringList>

#define DB_CONSTANT 0.75257498915995298803434723681123//1.5051499783199059760686944736225
#define OFFSET_CONSTANT_ONLINE -158.69

#define OFFSET_CONSTANT_OFFLINE -119
#define PRI 1e-3
#define MaximumCalibrationSize		28
#define StartFrequencyCalibration	20


#define KBYTE			0x400
#define MBYTE			0x100000
#define GBYTE			0x40000000

#define SYNC1	0xAA
#define SYNC2	0xBB

#define Registers_Packet 0x01

#define sizeOfOneEncPacket 1072

#define BaudRateCommand					0x01
#define TxPowerCommand					0x02


#define TxFrequencyCommand				0x05
#define RxFrequencyCommand				0x06
#define TxShortPNCommand				0x07
#define RxShortPNCommand				0x08
#define TxAESkeyCommand					0x09
#define RxAESkeyCommand					0x0A

#define TxGetAESkeyCommand				0x0C
#define ModulationModeCommand			0x0D
#define CodingRateCommand				0x0E
#define ProcessingGainCommand			0x0F
#define SelectModulationCommand			0x0F  //must be changed

#define BandwidthCommand				0x11 
#define TxLanOnCommand					0x12
#define RxLanOnCommand					0x13
#define VideoSourceCommand				0x14
#define SatteliteIDCommand				0x15			
#define	LnbIDCommand					0x16
#define CompressionRatioCommand			0x17
#define FrameDropCommand				0x18
#define SateliteAxesCommand				0x19
#define AttenationCommand				0x1A
#define GetBUCCommand					0x1B
#define StandbyCommand					0x1C
#define OffCommand						0x1D
#define TransmitCommand					0x1E
#define EnableRFCommand					0x1F
#define InhibitRFCommand				0x20
#define CoarseScanCommand				0x21
#define FineScanCommand					0x22
#define PMUOnCommand					0x23 
#define WorkingModeCommand				0x24
#define AutoCommand						0x25 
#define RemoveAutoCommand				0x26
#define SatelitesAxesManualCommand		0x27
#define H264VideoRateCommand			0x28
#define TxOnCommand						0x29
#define PatternEnableCommand			0x2A
#define TxLongPNCommand					0x2B 
#define TxLongPNStateCommand			0x2C 
#define RxLongPNCommand					0x2D 
#define DopplerEnableCommand			0x2E 
#define SetCalibrationCommand			0x2F
#define SaveCalibrationCommand			0x30
#define SavedBnPower					0x31

#define CameraPasswordkeyCommand		0x32

#define RxBandwidthCommand				0x33
#define	RxCodingRateCommand				0x34
#define BWdividerCommwnd				0x35
#define RxBWdividerCommwnd				0x36
#define SlaveIdCommand					0x37
#define LinkParametersModeCommand		0x38


#define firstChk 0x65
#define secondChk 0x94

#define txSet 0x02
#define txInc 0x03
#define txDec 0x04
#define hzSet 0x05
#define hzInc 0x06
#define hzDec 0x07
#define reqStatus 0xF0
#define resModem 0xE0

typedef struct
{
	quint8  unprocessed : 1, data24Bit : 1, reserved1: 6;
	quint8	samplingFrequency;
	quint8  bandwidth;
	quint8	reserved2;
	float	centerFrequency;
}FileHeader;

typedef struct 
{
	quint8  data24Bit : 1, reserved1 : 7;
	quint8	samplingFrequency;
	quint8  bandwidth;
	quint8	reserved2;

	float	centerFrequency;

	quint32   reservedInt1;
	quint32   reservedInt2;
	quint32   reservedInt3;
	quint32   reservedInt4;
	quint32   reservedInt5;
	quint32   reservedInt6;
}FileInfo;

enum ChameleonMode{ Auto = 0, Manual = 1 };

enum ChameleonType{ SingleToneManual = 0, BarrageManual = 1, SweepManual = 2, MultipleToneManual = 3, LookThoroughAuto = 4, BarrageAuto = 5,FollowerAuto = 6, SweepAuto = 7 };

enum ChameleonUserPermission{ AdministartorLevel = 100 , UserLevel1 = 200 , UserLevel2 = 300 };

enum SocketConfigStatus{IDLE = 0,READ = 1, WRITE=2};
enum ModulationMode{ Normal_Mode = 0, FM_Mode = 1, LFM_Mode = 2, AM_Mode = 3 };
enum State_Type1 { WAITING_SYNC1, WAITING_SYNC2, WAITING_DEVICEID, WAITING_BYTE_COUNT, WAITING_SUB_TYPE, WAITING_SEQUENCE, WAITING_DATA, WAITING_CHECKSUM };
enum Loop_State { idleAck,sigAck1, sigAck2, commandAck, dataAck,msbAck,lsbAck};
enum State_Type { silence,processing,failure,voided,waiting,ready };
enum Prompt_Type { empty,wDone,wRes,txSetDone,txIncDone,txDecDone,hzSetDone,hzIncDone,hzDecDone };
enum Prompt_Waiting_Type { none,statW,txSetW,txIncW,txDecW,hzSetW,hzIncW,hzDecDW };
enum Flag_State { initial,awaitingActivity,awaitingResponse,completed,awaitingCompletion };
enum RecordAndPlayState{ IDLE_REC_PALY = 0, RECORD = 1, PLAY = 2, PAUSE = 3, STOP = 4 };

enum State_Test_Type { WAITING_TEST_SYNC1, WAITING_TEST_SYNC2, WAITING_TEST_SYNC3, WAITING_TEST_SYNC4, WAITING_TEST_DATA };

enum RecieverTestState{ StopMode = 0, CounterMode = 2, FileMode = 3,  FrameMode= 1};
typedef struct
{
	QString					userName;
	QString					firstName;
	QString					lastName;
	qint64					groupID;
	qint64					personalID;
	ChameleonUserPermission	userPermission;
	QString					userPassword;
}UserInformation;

//typedef enum MultipleToneChameleonType{Synch = 0, Asych = 1};

#endif // NEWTYPE_H
