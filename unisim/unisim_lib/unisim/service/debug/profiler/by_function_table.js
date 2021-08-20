// needs unisim/service/http_server/embedded_script.js

GUI.prototype.min_width = 500;

GUI.prototype.on_resize = function(width, height)
{
	width = Math.max(width, this.min_width);
	var content = document.querySelector('#content');
	var summary = document.querySelector('#summary');
	
	if(content)
	{
		content.style.width = width + 'px';
	}
	if(summary)
	{
		summary.style.width = width + 'px';
	}
}

var gui = new GUI();
