GUI.prototype.window_resize_polling_period = 500;
GUI.prototype.window_inner_width = 0;
GUI.prototype.window_inner_height = 0;
GUI.prototype.vertical_scrollbar_width = 24;
GUI.prototype.horizontal_scrollbar_height = 0;
GUI.prototype.magic = 0xC0CA;
GUI.prototype.timeout = null;

function GUI()
{
	document.addEventListener('DOMContentLoaded', this.on_dom_content_loaded.bind(this));
	window.addEventListener('unload', this.on_window_unload.bind(this));
}

GUI.prototype.within_tiled_gui = function()
{
	return window.parent.gui && (window.parent.gui.magic == 0xCAFE);
}

GUI.prototype.find_own_tab = function()
{
	return this.within_tiled_gui() && window.parent.gui.find_tab_by_iframe_name(window.name);
}

GUI.prototype.storage_item_prefix = function()
{
	var own_tab = this.find_own_tab();
	return own_tab ? (own_tab.tab_config.name + '.') : '';
}

GUI.prototype.save_window_scroll_top = function()
{
	if(!this.within_tiled_gui())
	{
		// stand-alone
		sessionStorage.setItem(window.name + '.window.scrollY', window.scrollY);
		sessionStorage.setItem(window.name + '.window.scrollX', window.scrollX);
	}
}

GUI.prototype.restore_window_scroll_top = function()
{
	if(!this.within_tiled_gui())
	{
		// stand-alone
		var y = sessionStorage.getItem(window.name + '.window.scrollY');
		var x = sessionStorage.getItem(window.name + '.window.scrollX');
		if(x && y)
		{
			window.scroll(x, y);
		}
	}
}

GUI.prototype.reload = function()
{
	window.location.replace(window.location.href); // reload without POST
}

GUI.prototype.auto_reload = function(period, mode)
{
	var own_tab = this.find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		own_tab.auto_reload(period, mode);
	}
	else
	{
		// stand-alone
		if(this.timeout)
		{
			clearTimeout(this.timeout);
		}
		if(period)
		{
			this.timeout = setTimeout(this.reload.bind(this), period);
		}
	}
}

GUI.prototype.find_tab_by_uri = function(uri_str)
{
	return this.within_tiled_gui() ? window.parent.gui.find_tab_by_uri(uri_str) : null;
}

GUI.prototype.auto_reload_tab_by_uri = function(uri_str, period, mode)
{
	var tab = this.find_tab_by_uri(uri_str);
	if(tab)
	{
		tab.auto_reload(period, mode);
	}
}

GUI.prototype.get_next_target = function()
{
	var own_tab = this.find_own_tab();
	return own_tab ? own_tab.get_next_target(window.name) : '_self';
}

GUI.prototype.on_resize = function(width, height)
{
}

GUI.prototype.on_load = function()
{
}

GUI.prototype.__on_load__ = function()
{
	this.window_resize_poller();
	setInterval(this.window_resize_poller.bind(this), this.window_resize_polling_period);
	this.on_load();
}

GUI.prototype.on_unload = function()
{
}

GUI.prototype.__on_unload__ = function()
{
	if(this.timeout)
	{
		clearTimeout(this.timeout);
		this.timeout = null;
	}
	this.on_unload();
}

GUI.prototype.window_resize_poller = function()
{
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != this.window_inner_width) || (new_window_inner_height != this.window_inner_height))
	{
		this.window_inner_width = new_window_inner_width;
		this.window_inner_height = new_window_inner_height;
		var width = Math.max(0, this.window_inner_width - this.vertical_scrollbar_width);
		var height = Math.max(0, this.window_inner_height - this.horizontal_scrollbar_height);
		this.on_resize(width, height);
	}
}

GUI.prototype.on_dom_content_loaded = function()
{
	if(!this.within_tiled_gui())
	{
		// stand-alone
		this.restore_window_scroll_top();
		this.__on_load__();
	}
}

GUI.prototype.on_window_unload = function()
{
	if(!this.within_tiled_gui())
	{
		// stand-alone
		this.save_window_scroll_top();
		this.__on_unload__();
	}
}

GUI.prototype.find_statusbar_item_by_name = function(name)
{
	if(this.within_tiled_gui())
	{
		return window.parent.gui.find_statusbar_item_by_name(name);
	}
	
	return null;
}

GUI.prototype.create_context_menu = function(x, y, context_menu_items, action_this, on_destroy_action)
{
	var own_tab = this.find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		var own_tab_pos = own_tab.get_pos();
		window.parent.gui.create_context_menu(own_tab_pos.x + x, own_tab_pos.y + y, context_menu_items, action_this, on_destroy_action);
	}
}
