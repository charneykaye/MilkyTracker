/*
 *  milkyplay/LoaderPTM.cpp
 *
 *  Copyright 2008 Peter Barth
 *
 *  This file is part of Milkytracker.
 *
 *  Milkytracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Milkytracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Milkytracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 *  LoaderPTM.cpp
 *  MilkyPlay Module Loader: PolyTracker
 */
#include "Loaders.h"

const char* LoaderPTM::identifyModule(const mp_ubyte* buffer)
{
	// check for .PTM module
	if (!memcmp(buffer+44,"PTMF",4)) 
	{
		return "PTM";
	}

	// this is not an .PTM
	return NULL;
}

static mp_sint32 convertPTMPattern(TXMPattern* XMPattern,
							 const mp_ubyte* srcPattern,
							 mp_uint32 maxChannels,
							 mp_sint32 patCnt)
{
	
	XMPattern->channum = maxChannels;
	XMPattern->effnum = 2;
	XMPattern->rows = 64;
	XMPattern->patternData = new mp_ubyte[maxChannels*6*64];
	
	if (XMPattern->patternData == NULL)
	{
		return -1;
	}
	
	memset(XMPattern->patternData,0,maxChannels*6*64);
	
	mp_ubyte* dstSlot = XMPattern->patternData;
	for (mp_sint32 row = 0; row < 64; row++)
		for (mp_uint32 c = 0; c < maxChannels; c++)
		{
			const mp_ubyte* srcSlot = srcPattern+row*32*5+c*5;
			
			dstSlot[0] = (srcSlot[0]==254 ? (mp_ubyte)XModule::NOTE_CUT : srcSlot[0]);
			
			dstSlot[1] = srcSlot[1];
			
			if (srcSlot[2]<=64)
			{				
				dstSlot[2] = 0xC;
				dstSlot[3] = XModule::vol64to255(srcSlot[2]);
			}
			
			mp_ubyte eff = srcSlot[3];
			mp_ubyte op = srcSlot[4];
						
			mp_ubyte nEff = 0;
			mp_ubyte nOp = 0;
			
			switch (eff)
			{
				// arpeggio
				case 0x00:
					if (op)
					{
						nEff = 0x20;
						nOp = op;
					}
					break;

				// porta up
				case 0x01:
					nEff = 0x47;
					nOp = op;
					break;

				// porta down
				case 0x02:
					nEff = 0x48;
					nOp = op;
					break;
				
				// tone porta
				case 0x03:
					nEff = 0x03;
					nOp = op;
					break;
				
				// vibrato
				case 0x04:
					nEff = 0x04;
					nOp = op;
					break;
					
				// tone porta & volslide
				case 0x05:
					nEff = 0x05;
					nOp = op;
					break;

				// vibrato & volslide
				case 0x06:
					nEff = 0x06;
					nOp = op;
					break;

				// tremolo
				case 0x07:
					nEff = 0x07;
					nOp = op;
					break;
				
				// set sample offset
				case 0x09:
					nEff = 0x09;
					nOp = op;
					break;

				// volume slide
				case 0x0A:
					nEff = 0x49;
					nOp = op;
					break;
				
				// Jump to order
				case 0x0B:
					nEff = 0x0B;
					nOp = op;
					break;
				
				// Jump to order
				case 0x0C:
					nEff = 0x0C;
					nOp = XModule::vol64to255(op);
					break;
				
				// Pattern break
				case 0x0D:					
					nEff = 0x0D;
					nOp = op;
					break;

				// PT subeffects
				case 0x0E:
					nEff=(op>>4)+0x30;
					nOp = op&0xf;
					
					if (nEff == 0x38)
					{
						nEff = 0x08;
						nOp<<=4;
					}
					
					break;

				// Set speed 
				case 0x0F:
					nEff = 0x0F;
					nOp = op;
					break;
																											
				// set global volume
				case 0x10:
					nEff = 0x10;
					nOp = XModule::vol64to255(op);
					break;
															
				// retrig
				case 0x11:
					nEff = 0x1B;
					nOp = op;
					break;
										
				// fine vibrato
				case 0x12:
					nEff = 0x4A;
					nOp = op;
					break;
					
					
				default:
#ifdef VERBOSE
					if (eff!=255)
					{
						printf("Unsupported command: %x,%x\n",eff,op);
					}
#endif
					break;
			}
			
			dstSlot[4] = nEff;
			dstSlot[5] = nOp;
			
			dstSlot+=6;
		}
			
		return 0;	
	
}

mp_sint32 LoaderPTM::load(XMFileBase& f, XModule* module)
{
	mp_ubyte orders[256];
	mp_uword patParaPtrs[129];
	mp_uint32 samplePtrs[256];
	
	module->cleanUp();

	// this will make code much easier to read
	TXMHeader*		header = &module->header;
	TXMInstrument*	instr  = module->instr;
	TXMSample*		smp	   = module->smp;
	TXMPattern*		phead  = module->phead;	

	// we're already out of memory here
	if (!phead || !instr || !smp)
		return -7;
	
	f.read(&header->name,1,28);
	header->whythis1a = f.readByte();
	
	if (f.readWord() != 0x203) return -8;	// can't read this one
	
	f.readByte(); // skip something
	
	header->ordnum = f.readWord(); // number of positions in order list (songlength)
	
	header->insnum = f.readWord(); // number of instruments
	header->smpnum = header->insnum;
	header->patnum = f.readWord(); // number of patterns	
	
	header->channum = f.readWord();

	f.readDword();

	f.read(header->sig,1,4);
	
	f.readDword();
	f.readDword();
	f.readDword();
	f.readDword();

	mp_ubyte channelSettings[32];
	f.read(channelSettings,1,32);
	
	header->flags = XModule::MODULE_ST3NEWINSTRUMENT; // not sure about this though
	header->mainvol = 255; // initial main volume
	header->tempo = 6; // tempo
	header->speed = 125; // speed
	
	f.read(orders,1,256);
	
	mp_sint32 i,j;
	for (i = 0; i < header->ordnum; i++)
		header->ord[i] = orders[i];
	
	f.readWords(patParaPtrs,128);
	
	//////////////////////
	// read instruments //
	//////////////////////
	
	memset(samplePtrs,0,sizeof(mp_uint32)*header->insnum);
	
	bool bPatOffsetWorkaround = false;

	mp_sint32 s = 0;
	for (i = 0; i < header->insnum; i++)
	{
		mp_ubyte type = f.readByte();
		
		f.read(smp[s].name,1,12);	// read dos filename

		smp[s].vol = module->vol64to255(f.readByte());

		mp_uint32 c4spd = f.readWord();
		
		module->convertc4spd(c4spd,&smp[s].finetune,&smp[s].relnote);		
		
		f.readWord();

		// stupid fileoffsets
		samplePtrs[s] = f.readDword();
		
		// pattern size workaround / see ptm doc for details
		if (samplePtrs[s] && !bPatOffsetWorkaround)
		{
			patParaPtrs[header->patnum] = samplePtrs[i]>>4; 
			bPatOffsetWorkaround = true;
		}

		mp_sint32 correct = ((type&16)>>4)+1;

		smp[s].samplen = f.readDword();
		smp[s].loopstart = f.readDword();
		mp_sint32 looplen = ((mp_sint32)f.readDword() - correct - (mp_sint32)smp[s].loopstart);
		if (looplen < 0) 
			looplen = 0;
		smp[s].looplen = looplen;

		smp[s].flags = 1;
		smp[s].pan = 0x80;
		
		f.readDword();	// skip gus stuff
		f.readDword();
		f.readDword();
		f.readByte(); 
		
		f.readByte();
		
		f.read(instr[i].name,1,28); // instrument name
		
		f.readDword(); // should be 0x534d5450, can also be 0

		// looping
		if (type & 4)
		{
			if (type & 8)
				smp[s].type = 2;
			else
				smp[s].type = 1;
		}
		
		// 16 bit sample
		if (type & 16)
		{
			smp[s].type |= 16;
			smp[s].samplen >>= 1;
			smp[s].loopstart >>= 1;
			smp[s].looplen >>= 1;
		}
			
		if ((type & 3) && (smp[s].samplen != 0))
		{
			instr[i].samp=1;
			for (j=0;j<120;j++) 
				instr[i].snum[j] = s;
				
			s++;
		}
					
	}
	
	header->smpnum = s;
	
	//////////////////////
	// read patterns	//
	//////////////////////
	mp_ubyte* pattern = new mp_ubyte[64*32*5];
	if (pattern == NULL)
	{
		return -7;
	}
	
	for (i = 0; i < header->patnum; i++)
	{
		for (j = 0; j < 32*64; j++)
		{
			pattern[j*5] = 0x00;
			pattern[j*5+1] = 0;
			pattern[j*5+2] = 0xFF;
			pattern[j*5+3] = 0;
			pattern[j*5+4] = 0;
		}
		
		mp_uint32 patOffs = patParaPtrs[i]*16;
		
		mp_uint32 maxChannels = 1;
		
		if (patOffs)
		{
			
			f.seekWithBaseOffset(patOffs);
			
			mp_sint32 size = (patParaPtrs[i+1]-patParaPtrs[i])<<4;
			
			mp_ubyte* packed = new mp_ubyte[size];
			if (packed == NULL)
			{
				delete[] pattern;
				return -7;				
			}
			
			f.read(packed,1,size);
			
			mp_sint32 index = 0;
			mp_uint32 row = 0;
			
			while (index<size)
			{
				
				mp_ubyte pi = packed[index++];
				
				if (pi == 0) {
					row++;
					continue;
				}
				
				mp_uint32 chn = pi&31;
				
				if (chn>maxChannels)
					maxChannels = chn;
				
				mp_ubyte* slot = pattern+(row*32*5)+chn*5;
				
				if (pi & 32)
				{
					if (row<64)
					{
						slot[0] = packed[index++];
						slot[1] = packed[index++];
					}
					else index+=2;
				}
				if (pi & 64)
				{
					if (row<64)
					{
						slot[3] = packed[index++];
						slot[4] = packed[index++];
					}
					else index+=2;
				}
				if (pi & 128)
				{
					if (row<64)
						slot[2] = packed[index++];
					else
						index++;
				}
				
			}
			
			maxChannels++;
			
			if (maxChannels > header->channum)
				maxChannels = header->channum;
			
			delete[] packed;
			
		}
		
		convertPTMPattern(&phead[i],pattern,maxChannels,i);
		
	}
	
	delete[] pattern;
	
	for (i = 0; i < header->smpnum; i++)
	{
		mp_uint32 smpOffs = samplePtrs[i];
		
		if (smpOffs)
		{
			f.seekWithBaseOffset(smpOffs);
			
			if (module->loadModuleSample(f, i, XModule::ST_DELTA, XModule::ST_16BIT | XModule::ST_DELTA_PTM) != 0)
				return -7;
		}
	}
	
	strcpy(header->tracker,"Polytracker");
	
	module->setDefaultPanning();
	
	module->postProcessSamples();
	
	return 0;	
}
