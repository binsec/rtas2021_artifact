// needs unisim/service/http_server/embedded_script.js

GUI.prototype.on_resize = function(width, height)
{
	var var_table = document.querySelector('table.var-table');
	if(var_table)
	{
		var var_table_width = width - 32;
		var_table.style.width = var_table_width + 'px';
		var_table.style.maxWidth = var_table_width + 'px';
	}
}

GUI.prototype.on_load = function()
{
//	console.log('load ' + window.name);
	this.set_form_target();
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

var gui = new GUI();
