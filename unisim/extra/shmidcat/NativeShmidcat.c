/*
 *  Copyright (c) 2011,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Reda Nouacer <reda.nouacer@cea.fr> 
 */

#include <jni.h>

#include "NativeShmidcat.h"
#include "shmidcat.h"

/*
 * Class:     NativeShmidcat
 * Method:    shmidcat_init
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_unisim_workbench_simulation_services_instruments_gtkwave_model_NativeShmidcat_shmidcat_1init
  (JNIEnv *env, jobject jobj, jstring gtk_wave_path)

{

	jboolean iscopy;
	const char* cc = (*env)->GetStringUTFChars(env, gtk_wave_path, &iscopy);
	return shmidcat_init((char*) cc);
}

/*
 * Class:     NativeShmidcat
 * Method:    shmidcat_emit_string
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_unisim_workbench_simulation_services_instruments_gtkwave_model_NativeShmidcat_shmidcat_1emit_1string
  (JNIEnv *env, jobject jobj, jstring s)

{
	jboolean iscopy;
	const char* cc = (*env)->GetStringUTFChars(env, s, &iscopy);

	shmidcat_emit_string((char*) cc);
}

/*
 * Class:     NativeShmidcat
 * Method:    shmidcat_exit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_unisim_workbench_simulation_services_instruments_gtkwave_model_NativeShmidcat_shmidcat_1exit
  (JNIEnv *env, jobject jobj)

{
	shmidcat_exit();
}

