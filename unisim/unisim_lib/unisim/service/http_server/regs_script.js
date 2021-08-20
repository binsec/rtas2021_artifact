// needs unisim/service/http_server/embedded_script.js

GUI.prototype.on_resize = function(width, height)
{
	var reg_table = document.querySelector('table.reg-table');
	if(reg_table)
	{
		var reg_table_width = width - 8;
		reg_table.style.width = reg_table_width + 'px';
		reg_table.style.maxWidth = reg_table_width + 'px';
	}
}

GUI.prototype.on_load = function()
{
//	console.log('load ' + window.name);
	this.set_form_target();
	this.restore_collapsibles();
}

GUI.prototype.on_unload = function()
{
	this.save_collapsibles();
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

GUI.prototype.restore_collapsibles = function()
{
	var collapse_toggles = document.getElementsByClassName('collapse-toggle');
	if(collapse_toggles)
	{
		for(var i = 0; i < collapse_toggles.length; i++)
		{
			var collapse_toggle = collapse_toggles[i];
			var reg_details = collapse_toggle.parentNode.parentNode.parentNode.nextElementSibling;
			var collapse_toggle_state = sessionStorage.getItem(this.storage_item_prefix() + collapse_toggle.id + '-state');
			if(collapse_toggle_state == 'fold')
			{
				collapse_toggle.classList.remove('unfold');
				collapse_toggle.classList.add('fold');
				if(reg_details)
				{
					reg_details.style.display = 'table-row-group';
				}
			}
			else
			{
				collapse_toggle.classList.remove('fold');
				collapse_toggle.classList.add('unfold');
				if(reg_details)
				{
					reg_details.style.display = 'none';
				}
			}
			collapse_toggle.addEventListener(
				'click',
				function()
				{
					var reg_details = this.parentNode.parentNode.parentNode.nextElementSibling;
					if(reg_details)
					{
						if(this.classList.contains('unfold'))
						{
							this.classList.remove('unfold');
							this.classList.add('fold');
							reg_details.style.display = 'table-row-group';
							sessionStorage.setItem(gui.storage_item_prefix() + this.id + '-state', 'fold');
						}
						else
						{
							this.classList.remove('fold');
							this.classList.add('unfold');
							reg_details.style.display = 'none';
							sessionStorage.setItem(gui.storage_item_prefix() + this.id + '-state', 'unfold');
						}
					}
				}
			);
		}
	}
}

GUI.prototype.save_collapsibles = function()
{
	var collapse_toggles = document.getElementsByClassName('collapse-toggle');
	if(collapse_toggles)
	{
		for(var i = 0; i < collapse_toggles.length; i++)
		{
			var collapse_toggle = collapse_toggles[i];
			if(collapse_toggle.classList.contains('fold'))
			{
				sessionStorage.setItem(this.storage_item_prefix() + collapse_toggle.id + '-state', 'fold');
			}
			else
			{
				sessionStorage.setItem(this.storage_item_prefix() + collapse_toggle.id + '-state', 'unfold');
			}
		}
	}
}

var gui = new GUI();
