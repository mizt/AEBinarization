/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "Binarization.h"

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_SPRINTF(out_data->return_msg,
				"%s\n%s",
				NAME,
				DESCRIPTION);

	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err	err = PF_Err_NONE;
 
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);
	 
	out_data->out_flags |=	PF_OutFlag_PIX_INDEPENDENT |
							PF_OutFlag_USE_OUTPUT_EXTENT;

	return err;
}

static PF_Err 
ParamsSetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err			err = PF_Err_NONE;
	PF_ParamDef		def;
    
    AEFX_CLR_STRUCT(def);
    PF_ADD_SLIDER("threshold",0,256,0,256,128,BINARIZATION_THRESHOLD);

	out_data->num_params = BINARIZATION_NUM_PARAMS;

	return err;
}

static PF_Err 
SequenceSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
    
	return PF_Err_NONE;
}

static PF_Err 
SequenceSetdown (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	return PF_Err_NONE;
}

static PF_Err 
SequenceResetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	return PF_Err_NONE;
}

static PF_Err 
Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
    unsigned int threshold = params[BINARIZATION_THRESHOLD]->u.sd.value;
    
    int width  = output->width;
    int height = output->height;
    
    PF_EffectWorld *input = &params[0]->u.ld;
    
    unsigned int *src = (unsigned int *)input->data;
    int srcRow = input->rowbytes>>2;
    
    unsigned int *dst = (unsigned int *)output->data;
    int dstRow = output->rowbytes>>2;

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            
            unsigned int pix = src[i*srcRow+j];
            
            unsigned char r = (pix>>8)&0xFF;
            unsigned char g = (pix>>16)&0xFF;
            unsigned char b = (pix>>24)&0xFF;
            
            unsigned char gray = 0.299*r + 0.587*g + 0.114*b;
            unsigned char o1o1 = (gray>=threshold)?255:0;
            
            dst[i*dstRow+j] = 0x00000FF|o1o1<<24|o1o1<<16|o1o1<<8;
        }
    }
    
	return PF_Err_NONE;
}

PF_Err
EffectMain (	
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err = PF_Err_NONE;
	
	switch (cmd) {
	case PF_Cmd_ABOUT:
		err = About(in_data,out_data,params,output);
		break;
	case PF_Cmd_GLOBAL_SETUP:
		err = GlobalSetup(in_data,out_data,params,output);
		break;
	case PF_Cmd_PARAMS_SETUP:
		err = ParamsSetup(in_data,out_data,params,output);
		break;
	case PF_Cmd_SEQUENCE_SETUP:
		err = SequenceSetup(in_data,out_data,params,output);
		break;
	case PF_Cmd_SEQUENCE_SETDOWN:
		err = SequenceSetdown(in_data,out_data,params,output);
		break;
	case PF_Cmd_SEQUENCE_RESETUP:
		err = SequenceResetup(in_data,out_data,params,output);
		break;
	case PF_Cmd_RENDER:
		err = Render(in_data,out_data,params,output);
		break;
	}
	return err;
}

#ifdef AE_OS_WIN
	BOOL WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
	{
		HINSTANCE my_instance_handle = (HINSTANCE)0;
		
		switch (dwReason)
		{
			case DLL_PROCESS_ATTACH:
			my_instance_handle = hDLL;
				break;

			case DLL_THREAD_ATTACH:
			my_instance_handle = hDLL;
				break;
			case DLL_THREAD_DETACH:
			my_instance_handle = 0;
				break;
			case DLL_PROCESS_DETACH:
			my_instance_handle = 0;
				break;
					break;
		}
		return(TRUE);
	}
#endif

