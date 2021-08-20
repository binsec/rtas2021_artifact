// needs unisim/service/http_server/embedded_script.js

GUI.prototype.min_width = 830;

GUI.prototype.on_resize = function(width, height)
{
	width = Math.max(width, this.min_width);
	var content = document.querySelector('#content');
	var var_index = document.querySelector('#var-index');
	
	if(content)
	{
		content.style.width = width + 'px';
	}
	if(var_index)
	{
		var_index.style.width = width + 'px';
	}
}

var gui = new GUI();
