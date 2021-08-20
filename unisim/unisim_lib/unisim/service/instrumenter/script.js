// needs unisim/service/http_server/embedded_script.js

GUI.prototype.min_scroller_height = 76;

GUI.prototype.save_instrumenter_scroller_scroll_top = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
//		console.log(window.name + ':' + this.storage_item_prefix() + 'div.scroller.scrollTop <- ' + el.scrollTop);
		sessionStorage.setItem(this.storage_item_prefix() + 'div.scroller.scrollTop', el.scrollTop);
	}
}

GUI.prototype.restore_instrumenter_scroller_scroll_top = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		var t = sessionStorage.getItem(this.storage_item_prefix() + 'div.scroller.scrollTop');
		if(t)
		{
//			console.log(window.name + ':' + this.storage_item_prefix() + 'div.scroller.scrollTop = ' + t);
			el.scrollTop = t;
		}
	}
}

GUI.prototype.on_load = function()
{
//	console.log('load ' + window.name);
	this.set_form_target();
	this.restore_instrumenter_scroller_scroll_top();
	this.set_status();
}

GUI.prototype.on_unload = function()
{
//	console.log('unload ' + window.name);
	this.save_instrumenter_scroller_scroll_top();
}

GUI.prototype.set_form_target = function()
{
	var forms = document.getElementsByTagName('form');
	if(forms)
	{
		var target = this.get_next_target();
		for(var i = 0; i < forms.length; i++)
		{
			var form = forms[i];
// 		console.log('form target set to ' + target);
			form.setAttribute('target', target);
		}
	}
}

GUI.prototype.on_resize = function(width, height)
{
	var scroller = document.querySelector('div.scroller');
	if(scroller)
	{
		var rect = scroller.getBoundingClientRect();
		var scroller_height = Math.max(Math.floor(height - rect.top - 1), this.min_scroller_height);
		scroller.style.height = scroller_height + 'px';
	}
}

var gui = new GUI();
