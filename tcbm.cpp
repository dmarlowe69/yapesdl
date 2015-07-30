#include "tcbm.h"

void CFakeTCBM::Reset()
{
	State = IEC_IDLE;
	Data = HandShake = Status = 0;
	tia.ddra = tia.ddrb = tia.ddrc = 0;
	tia.pra = tia.prb = tia.prc = 0;
}

unsigned char CFakeTCBM::Read(unsigned int addr)
{
	//unsigned char drive = (addr&0x0030)>>5;

	switch (addr&7) {
		case 0:
			return (Data & ~tia.ddra)
					|(tia.pra & tia.ddra);
        case 1:
			return (Status & ~tia.ddrb)
					|(tia.prb & tia.ddrb);
        case 2:
			return (HandShake & ~tia.ddrc)
					|(tia.prc & tia.ddrc);
        case 3:
			return tia.ddra;
		case 4:
			return tia.ddrb;
		case 5:
			return tia.ddrc;
		case 6:
		    return tia.cr;
        default:
		case 7:
			return 0xFF;
	}
}

void CFakeTCBM::Write( unsigned int addr, unsigned char data)
{
	unsigned int drive = (addr & 0x0030) >> 5;

	switch (addr & 7) {
		// Port registers A-B-C
		case 0:
			tia.pra = data;
			switch (State) {
				case IEC_IDLE:
					switch(data & 0x0F) {
					    case 0x00:
					        break;
						case 0x01:	// Command w/ dev nr coming (TALK/LISTEN/UNTALK/UNLISTEN)
							State = IEC_COMMAND;
							break;
						case 0x02:  // Command w/ sec addr. coming (0x60, 0xE0, 0xF0)
							State = IEC_SECONDARY;
							break;
						case 0x03:	// Data goes to drive (filename, command, data)
							State = IEC_OUTPUT;
							break;
						case 0x04:  // Data comes (to plus/4)
							Status = iec->In( &Data ) ? TCBM_ERROR : TCBM_OK;
							break;
						case 0x05:	// Check device
							Status = TCBM_OK;
							break;
					};
					break;
				case IEC_COMMAND:
					Status = iec->OutCmd( data|(9 - drive)) ? TCBM_ERROR : TCBM_OK;
					State = IEC_IDLE;
					break;
				case IEC_SECONDARY:
					Status = iec->OutSec( data ) ? TCBM_ERROR : TCBM_OK;
					State = IEC_IDLE;
					break;
				case IEC_OUTPUT:
					Status = iec->Out( data ) ? TCBM_ERROR : TCBM_OK;
					State = IEC_IDLE;
					break;
			};
			break;
		case 1:
			tia.prb = data;
			break;
		case 2:
			tia.prc = data;
			HandShake = (((data&tia.ddrc)^0x40)<<1)&0x80;
			break;
		// Data direction registers A-B-C
		case 3:
			tia.ddra = data;
			break;
		case 4:
			tia.ddrb = data;
			break;
		case 5:
			tia.ddrc = data;
			break;
		case 6:
			tia.cr = data;
			break;
		default:
		case 7:
			break;
	};
}
