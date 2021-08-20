/*
 *  Copyright (c) 2019,
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
 */

#include <unisim/util/json/json.tcc>
#include <iostream>
#include <fstream>

int main(int argc, const char *argv[])
{
	std::ifstream *file = 0;
	std::istream *stream = &std::cin;
	
	if(argc >= 2)
	{
		file = new std::ifstream(argv[1]);
		stream = file;
	}

#if 1
	unisim::util::json::JSON_Parser<unisim::util::json::JSON_AST_Builder> json_parser;
	unisim::util::json::JSON_AST_Builder json_ast_builder;
	
	unisim::util::json::JSON_Value *root = json_parser.Parse(*stream, json_ast_builder);
	if(root)
	{
		unisim::util::json::JSON_AST_Printer json_ast_printer;
		json_ast_printer.Print(std::cout, *root);
		
		std::cout << std::endl << std::endl;
		std::cout << *root << std::endl;

		std::cerr << "Parse success" << std::endl;
		
		delete root;
	}
	else
	{
		std::cerr << "Parse error" << std::endl;
	}
#else
	unisim::util::json::JSON_Parser<unisim::util::json::JSON_Parser_Printer> json_parser;
	unisim::util::json::JSON_Parser_Printer json_parser_printer;
	
			
	if(!json_parser.Parse(*stream, json_parser_printer))
	{
		std::cerr << "Parse error" << std::endl;
	}
#endif

	if(file) delete file;
	return 0;
}
