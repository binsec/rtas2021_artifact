package org.unisim.workbench.simulation.services.instruments.gtkwave.model;
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


import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;


public class NativeShmidcat {

// Native method declaration
	native int shmidcat_init(String gtk_wave_path);
	native void shmidcat_emit_string(String s);
	native void shmidcat_exit();
	
// Load the library
	static {
		String osName = System.getProperty("os.name");
		if (osName.toLowerCase().lastIndexOf("windows") != -1) {
          	    System.loadLibrary("libshmidcat-0");
		} else if (osName.toLowerCase().lastIndexOf("linux") != -1) {
	            System.loadLibrary("shmidcat");
		} else {
		    System.err.println("Unknowen operating system. Failing to load shmidcat DLL");
		}
	}

	public NativeShmidcat() {
		
	}
	
	public void plotVCDFile(final String gtkwave_path, final String filename) {
		
		int shmid = shmidcat_init(gtkwave_path);

		Thread collectorThread = new Thread(new Runnable() {
			
			@Override
			public void run() {
				
				try {
					
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					
					BufferedReader bufferedReader = new BufferedReader(new FileReader(filename));
					String line = null;
					
					while ((line = bufferedReader.readLine()) != null) {
						shmidcat_emit_string(line+"\n");

					}
					
					bufferedReader.close();

//					shmidcat_exit();

				} catch (FileNotFoundException e) {
					e.printStackTrace();			
				} catch (IOException e) {
					e.printStackTrace();
				}
				
			}
		});
		
		collectorThread.start();

	}
	
	public static void main(String args[]) {
		if (args.length < 2) {
		    System.out.println("Usage: NativeShmidcat <gtkwave_exe_path> <vcd_file_path>");
		    return;
                }

		NativeShmidcat shmidcat = new NativeShmidcat();
		
		shmidcat.plotVCDFile(args[0], args[1]);
		
	}
}
