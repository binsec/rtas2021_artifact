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
 
#ifndef __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__
#define __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/hypapp/hypapp.hh>

namespace unisim {
namespace service {
namespace interfaces {

struct CSSFile
{
	CSSFile() : filename() {}
	CSSFile(const std::string& _filename) : filename(_filename) {}
	void SetFilename(const std::string& _filename) { filename = _filename; }
	const std::string& GetFilename() const { return filename; }
private:
	std::string filename;
};

struct JSFile
{
	JSFile() : filename() {}
	JSFile(const std::string& _filename) : filename(_filename) {}
	void SetFilename(const std::string& _filename) { filename = _filename; }
	const std::string& GetFilename() const { return filename; }
private:
	std::string filename;
};

struct JSAction
{
	enum Location
	{
		BROWSER,
		TOOLBAR
	};
	
	JSAction() : location(BROWSER), name(), js_code_snippet() {}
	JSAction(Location _location, const std::string& _name) : location(_location), name(_name), js_code_snippet() {} 
	JSAction(Location _location, const std::string& _name, const std::string& _js_code_snippet) : location(_location), name(_name), js_code_snippet(_js_code_snippet) {} 
	void SetLocation(Location _location) { location = _location; }
	void SetName(const std::string& _name) { name = _name; }
	void SetJSCodeSnippet(const std::string& _js_code_snippet) { js_code_snippet = _js_code_snippet; }
	Location GetLocation() const { return location; }
	const std::string& GetName() const { return name; }
	const std::string& GetJSCodeSnippet() const { return js_code_snippet; }
private:
	Location location;
	std::string name;
	std::string js_code_snippet;
};

struct OpenTabAction : JSAction
{
	enum Tile
	{
		LEFT_TILE,
		TOP_MIDDLE_TILE,
		TOP_RIGHT_TILE,
		BOTTOM_TILE
	};
	
	OpenTabAction()
		: JSAction()
		, tile(TOP_MIDDLE_TILE)
		, uri()
	{
	}
	
	OpenTabAction(Location location, const std::string& name, Tile _tile, const std::string& _uri)
		: JSAction(location, name)
		, tile(_tile)
		, uri(_uri)
	{
	}
	
	void SetTile(Tile _tile) { tile = _tile; }
	void SetURI(const std::string& _uri) { uri = _uri; }
	Tile GetTile() const { return tile; }
	const std::string& GetURI() const { return uri; }
private:
	Tile tile;
	std::string uri;
};

struct BrowserAction
{
	BrowserAction()
		: object_name()
		, label()
	{
	}
	
	virtual ~BrowserAction()
	{
	}
	
	BrowserAction(const std::string& _object_name, const std::string& _label)
		: object_name(_object_name)
		, label(_label)
	{
	}
	
	void SetObjectName(const std::string& _object_name) { object_name = _object_name; }
	void SetLabel(const std::string& _label) { label = _label; }
	const std::string& GetObjectName() const { return object_name; }
	const std::string& GetLabel() const { return label; }
	
	virtual const std::string& GetJSCodeSnippet() const
	{
		static std::string null_str;
		return null_str;
	}
private:
	std::string object_name;
	std::string label;
};

struct BrowserDoAction : BrowserAction, JSAction
{
	BrowserDoAction()
		: BrowserAction()
		, JSAction()
	{
	}

	BrowserDoAction(Location location, const std::string& name, const std::string& object_name, const std::string& label, const std::string& js_code_snippet)
		: BrowserAction(object_name, label)
		, JSAction(location, name, js_code_snippet)
	{
	}
	
	virtual const std::string& GetJSCodeSnippet() const
	{
		return JSAction::GetJSCodeSnippet();
	}
};

struct BrowserOpenTabAction : BrowserAction, OpenTabAction
{
	BrowserOpenTabAction()
		: BrowserAction()
		, OpenTabAction()
	{
	}
	
	BrowserOpenTabAction(const std::string& name, const std::string& object_name, const std::string& label, Tile tile, const std::string& uri)
		: BrowserAction(object_name, label)
		, OpenTabAction(BROWSER, name, tile, uri)
	{
	}
	
	BrowserOpenTabAction(const BrowserOpenTabAction& o, const std::string& js_code_snippet)
		: BrowserAction(o)
		, OpenTabAction(o)
	{
		SetJSCodeSnippet(js_code_snippet);
	}
	
	virtual const std::string& GetJSCodeSnippet() const
	{
		return JSAction::GetJSCodeSnippet();
	}
};

struct ToolbarAction
{
	ToolbarAction()
		: label()
	{
	}
	
	ToolbarAction(const std::string& _label, const std::string& _tips)
		: label(_label)
		, tips(_tips)
	{
	}
	
	virtual ~ToolbarAction()
	{
	}
	
	void SetLabel(const std::string& _label) { label = _label; }
	void SetTips(const std::string& _tips) { tips = _tips; }
	const std::string& GetLabel() const { return label; }
	const std::string& GetTips() const { return tips; }
	
	virtual const std::string& GetName() const
	{
		static std::string null_str;
		return null_str;
	}
	
	virtual const std::string& GetJSCodeSnippet() const
	{
		static std::string null_str;
		return null_str;
	}
private:
	std::string label;
	std::string tips;
};

struct ToolbarDoAction : ToolbarAction, JSAction
{
	ToolbarDoAction()
		: ToolbarAction()
		, JSAction()
	{
	}
	
	ToolbarDoAction(const std::string& name, const std::string& label, const std::string& tips)
		: ToolbarAction(label, tips)
		, JSAction(TOOLBAR, name)
	{
	}

	ToolbarDoAction(const std::string& name, const std::string& label, const std::string& tips, const std::string& js_code_snippet)
		: ToolbarAction(label, tips)
		, JSAction(TOOLBAR, name, js_code_snippet)
	{
	}
	
	virtual const std::string& GetName() const
	{
		return JSAction::GetName();
	}

	virtual const std::string& GetJSCodeSnippet() const
	{
		return JSAction::GetJSCodeSnippet();
	}
};

struct ToolbarOpenTabAction : ToolbarAction, OpenTabAction
{
	ToolbarOpenTabAction()
		: ToolbarAction()
		, OpenTabAction()
	{
	}
	
	ToolbarOpenTabAction(const std::string& name, const std::string& label, const std::string& tips, Tile tile, const std::string& uri)
		: ToolbarAction(label, tips)
		, OpenTabAction(TOOLBAR, name, tile, uri)
	{
	}
	
	ToolbarOpenTabAction(const ToolbarOpenTabAction& o, const std::string& js_code_snippet)
		: ToolbarAction(o)
		, OpenTabAction(o)
	{
		SetJSCodeSnippet(js_code_snippet);
	}
	
	virtual const std::string& GetName() const
	{
		return JSAction::GetName();
	}

	virtual const std::string& GetJSCodeSnippet() const
	{
		return JSAction::GetJSCodeSnippet();
	}
};

inline std::ostream& operator << (std::ostream& os, const JSAction& a)
{
	return os << "JSAction(location=" << a.GetLocation() << ",name=\"" << a.GetName() << "\",js_code_snippet=\"" << a.GetJSCodeSnippet() << "\")";
}

inline std::string to_string(const OpenTabAction::Tile& tile)
{
	switch(tile)
	{
		case OpenTabAction::LEFT_TILE      : return "left-tile";
		case OpenTabAction::TOP_MIDDLE_TILE: return "top-middle-tile";
		case OpenTabAction::TOP_RIGHT_TILE : return "top-right-tile";
		case OpenTabAction::BOTTOM_TILE    : return "bottom-tile";
		default                            : return "unknown-tile";
	}
	
	return "";
}

inline std::ostream& operator << (std::ostream& os, const OpenTabAction::Tile& tile)
{
	return os << to_string(tile);
}

inline std::ostream& operator << (std::ostream& os, const BrowserAction& a)
{
	return os << "BrowserAction(object_name=\"" << a.GetObjectName() << "\",label=\"" << a.GetLabel() << "\",js_code_snippet=\"" << a.GetJSCodeSnippet() << "\")";
}

struct StatusBarItem
{
	StatusBarItem() : name(), class_name(), html_code_snippet() {}
	StatusBarItem(const StatusBarItem& o) : name(o.name), class_name(o.class_name), html_code_snippet(o.html_code_snippet) {}
	StatusBarItem(const std::string& _name, const std::string& _class_name, const std::string& _html_code_snippet) : name(_name), class_name(_class_name), html_code_snippet(_html_code_snippet) {}
	void SetName(const std::string& _name) { name = _name; }
	void SetClassName(const std::string& _class_name) { class_name = _class_name; }
	void SetHTMLCodeSnippet(const std::string& _html_code_snippet) { html_code_snippet = _html_code_snippet; }
	const std::string& GetName() const { return name; }
	const std::string& GetClassName() const { return class_name; }
	const std::string& GetHTMLCodeSnippet() const { return html_code_snippet; }
private:
	std::string name;
	std::string class_name;
	std::string html_code_snippet;
};

struct WebInterfaceModdingScanner
{
	virtual void Append(const CSSFile&) = 0;              // include CSS file in main document HTML head section
	virtual void Append(const JSFile&) = 0;               // include JS file in main document HTML head section
	virtual void Append(const BrowserDoAction&) = 0;      // add JS code snippet in browser
	virtual void Append(const BrowserOpenTabAction&) = 0; // add open tab action in browser
	virtual void Append(const ToolbarDoAction&) = 0;      // add JS code snippet in toolbar
	virtual void Append(const ToolbarOpenTabAction&) = 0; // add open tab action in toolbar
	virtual void Append(const StatusBarItem&) = 0;        // add a status bar item
};

struct HttpServer : ServiceInterface
{
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn) = 0;
	virtual void ScanWebInterfaceModdings(WebInterfaceModdingScanner& scanner) = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_HTTP_SERVER_HH__

