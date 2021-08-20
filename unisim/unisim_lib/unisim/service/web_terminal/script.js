// needs unisim/service/http_server/embedded_script.js

GUI.prototype.dummy_iframe = null;
GUI.prototype.form = null;
GUI.prototype.input_key = null;
GUI.prototype.input_ctrl = null;
GUI.prototype.input_shift = null;
GUI.prototype.input_alt = null;
GUI.prototype.input_meta = null;
GUI.prototype.input_text = null;
GUI.prototype.screen_buffer = null;
GUI.prototype.bound_on_keypress = null;
GUI.prototype.bound_on_focus_in = null;
GUI.prototype.bound_on_focus_out = null;
GUI.prototype.bound_on_contextmenu = null;

GUI.prototype.on_load = function()
{
// 	console.log(window.name + ':load');
	
	this.dummy_iframe = document.createElement('iframe');
	this.dummy_iframe.setAttribute('name', 'dummy');
	this.dummy_iframe.setAttribute('src', 'about:blank');
	this.dummy_iframe.setAttribute('style', 'display:block;visibility:hidden;width:0px;height:0px;');
	document.body.appendChild(this.dummy_iframe);
	
	this.form = document.createElement('form');
	this.form.setAttribute('tabindex', '-1000');
	this.form.setAttribute('action', window.location.href);
	this.form.setAttribute('method', 'post');
	this.form.setAttribute('target', 'dummy');
	this.form.setAttribute('style', 'display:none');
	
	this.input_key = document.createElement('input');
	this.input_key.setAttribute('type', 'text');
	this.input_key.setAttribute('name', 'key');
	this.input_key.setAttribute('value', '');

	this.input_ctrl = document.createElement('input');
	this.input_ctrl.setAttribute('type', 'text');
	this.input_ctrl.setAttribute('name', 'ctrl');
	this.input_ctrl.setAttribute('value', '');

	this.input_shift = document.createElement('input');
	this.input_shift.setAttribute('type', 'text');
	this.input_shift.setAttribute('name', 'shift');
	this.input_shift.setAttribute('value', '');

	this.input_alt = document.createElement('input');
	this.input_alt.setAttribute('type', 'text');
	this.input_alt.setAttribute('name', 'alt');
	this.input_alt.setAttribute('value', '');

	this.input_meta = document.createElement('input');
	this.input_meta.setAttribute('type', 'text');
	this.input_meta.setAttribute('name', 'meta');
	this.input_meta.setAttribute('value', '');

	this.input_text = document.createElement('input');
	this.input_text.setAttribute('type', 'text');
	this.input_text.setAttribute('name', 'text');
	this.input_text.setAttribute('value', '');

	this.form.appendChild(this.input_key);
	this.form.appendChild(this.input_ctrl);
	this.form.appendChild(this.input_shift);
	this.form.appendChild(this.input_alt);
	this.form.appendChild(this.input_meta);
	this.form.appendChild(this.input_text);
	document.body.appendChild(this.form);

	this.screen_buffer = document.getElementById('screen-buffer');
	if(this.screen_buffer)
	{
		if(this.within_tiled_gui())
		{
			this.bound_on_contextmenu = function(event) { this.on_contextmenu(event); }.bind(this);
			this.screen_buffer.addEventListener('contextmenu', this.bound_on_contextmenu);
		}
		this.bound_on_keypress = function(event) { this.on_keypress(event); }.bind(this);
		this.screen_buffer.addEventListener('keydown', this.bound_on_keypress);
		var focused = !this.within_tiled_gui() || parseInt(sessionStorage.getItem(this.storage_item_prefix() + 'focused'));
		if(focused)
		{
// 			console.log(window.name + ':focus');
			this.screen_buffer.focus();
		}
		
		if(this.within_tiled_gui())
		{
			this.bound_on_focus_in = this.on_focus_in.bind(this);
			this.bound_on_focus_out = this.on_focus_out.bind(this);
			this.screen_buffer.addEventListener('focusin', this.bound_on_focus_in);
			this.screen_buffer.addEventListener('focusout', this.bound_on_focus_out);
		}
		else
		{
			this.bound_on_focus_in = null;
			this.bound_on_focus_out = null;
		}
	}
	else
	{
		this.bound_on_keypress = null;
		this.bound_on_focus_in = null;
		this.bound_on_focus_out = null;
	}
	if(activity)
	{
		var own_tab = this.find_own_tab();
		if(own_tab)
		{
			own_tab.scroll_to_bottom();
		}
	}
}

GUI.prototype.on_unload = function()
{
// 	console.log(window.name + ':unload');
	if(this.bound_on_focus_in)
	{
		this.screen_buffer.removeEventListener('focusin', this.bound_on_focus_in);
		this.bound_on_focus_in = null
	}
	if(this.bound_on_focus_out)
	{
		this.screen_buffer.removeEventListener('focusout', this.bound_on_focus_out);
		this.bound_on_focus_out = null;
	}
	if(this.screen_buffer)
	{
		this.screen_buffer.blur();
	}
	if(this.bound_on_contextmenu)
	{
		this.screen_buffer.removeEventListener('contextmenu', this.bound_on_contextmenu);
		this.bound_on_contextmenu = null;
	}
	if(this.bound_on_keypress)
	{
		this.screen_buffer.removeEventListener('keydown', this.bound_on_keypress);
		this.bound_on_keypress = null;
	}
}

GUI.prototype.on_keypress = function(event)
{
// 	console.log('keydown: key=' + event.key + ' ctrlKey=' + event.ctrlKey + ', shiftKey=' + event.shiftKey + ' altKey=' + event.altKey + ' metaKey=' + event.metaKey);
	this.send_key(event.key, event.ctrlKey, event.shiftKey, event.altKey, event.metaKey);
	event.preventDefault();
}

GUI.prototype.send_key = function(key, ctrl, shift, alt, meta)
{
	if(this.form && this.input_key && this.input_ctrl && this.input_shift && this.input_alt && this.input_meta && this.input_text)
	{
		this.input_key.setAttribute('value', key);
		this.input_ctrl.setAttribute('value', ctrl ? '1' : '0');
		this.input_shift.setAttribute('value', shift ? '1' : '0');
		this.input_alt.setAttribute('value', alt ? '1' : '0');
		this.input_meta.setAttribute('value', meta ? '1' : '0');
		this.input_text.setAttribute('value', '');
		this.form.submit();
	}
}

GUI.prototype.send_text = function(text)
{
	if(this.form && this.input_key && this.input_ctrl && this.input_shift && this.input_alt && this.input_meta && this.input_text)
	{
		this.input_key.setAttribute('value', '');
		this.input_ctrl.setAttribute('value', '0');
		this.input_shift.setAttribute('value', '0');
		this.input_alt.setAttribute('value', '0');
		this.input_meta.setAttribute('value', '0');
		this.input_text.setAttribute('value', text);
		this.form.submit();
	}
}

GUI.prototype.on_focus_in = function()
{
// 	console.log(window.name + ':focus in');
	sessionStorage.setItem(this.storage_item_prefix() + 'focused', 1);
}

GUI.prototype.on_focus_out = function()
{
// 	console.log(window.name + ':focus out');
	sessionStorage.setItem(this.storage_item_prefix() + 'focused', 0);
}

GUI.prototype.copy_screen_buffer_to_clipboard = function()
{
	if(navigator.clipboard && navigator.clipboard.writeText)
	{
		navigator.clipboard.writeText(this.screen_buffer.textContent).then(
			function()
			{
// 				console.log('done copy screen buffer to clipboard');
			},
			function()
			{
				console.log('can\'t copy screen buffer to clipboard');
			}
		);
	}
	else
	{
		console.log('missing support for navigator.clipboard.writeText');
	}
}

GUI.prototype.copy_clipboard_to_screen_buffer = function()
{
	if(navigator.clipboard && navigator.clipboard.readText)
	{
		navigator.clipboard.readText().then(
			function(text)
			{
				this.send_text(text);
// 				console.log('done copy clipboard to screen buffer');
			},
			function()
			{
				console.log('can\'t copy clipboard to screen buffer');
			}
		);
	}
	else
	{
		console.log('missing support for navigator.clipboard.readText');
	}
}

GUI.prototype.on_contextmenu = function(event)
{
	event.preventDefault();
	
	if(navigator.clipboard && (navigator.clipboard.writeText || navigator.clipboard.readText))
	{
		var own_tab = this.find_own_tab();
		if(own_tab)
		{
			own_tab.freeze();
		}
		
		var context_menu_items = new Array();

		if(navigator.clipboard.writeText)
		{
			context_menu_items.push(
				{
					label:'Copy screen to clipboard',
					action:function()
					{
// 						console.log('Copy screen to clipboard');
						if(navigator.permissions && navigator.permissions.query)
						{
							navigator.permissions.query({ name: 'clipboard-write', allowWithoutGesture:true }).then(
								function(result)
								{
									if((result.state == 'granted') || (result.state == 'prompt'))
									{
										this.copy_screen_buffer_to_clipboard();
									}
								}.bind(this),
								function()
								{
									console.log('can\'t query permission for \'clipboard-write\'');
									this.copy_screen_buffer_to_clipboard();
								}.bind(this)
							);
						}
						else
						{
							console.log('missing support for navigator.permissions.query');
							this.copy_screen_buffer_to_clipboard();
						}
					}
				}
			);
		}
		
		if(navigator.clipboard.readText)
		{
			context_menu_items.push(
				{
					label:'Paste clipboard as input',
					action:function()
					{
						if(navigator.permissions && navigator.permissions.query)
						{
							navigator.permissions.query({ name: 'clipboard-read', allowWithoutGesture:true }).then(
								function(result)
								{
									if((result.state == 'granted') || (result.state == 'prompt'))
									{
										this.copy_clipboard_to_screen_buffer();
									}
								}.bind(this),
								function()
								{
									console.log('can\'t query permission for \'clipboard-read\'');
									this.copy_clipboard_to_screen_buffer();
								}.bind(this)
							);
						}
						else
						{
							console.log('missing support for navigator.permissions.query');
							this.copy_clipboard_to_screen_buffer();
						}
					}
				}
			);
		}
		
		this.create_context_menu(
			event.clientX, event.clientY,
			context_menu_items,
			this,
			function()
			{
				var own_tab = this.find_own_tab();
				if(own_tab)
				{
					own_tab.unfreeze();
				}
			}
		);
	}
}

var gui = new GUI();
