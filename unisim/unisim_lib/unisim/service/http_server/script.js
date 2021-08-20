// needs unisim/service/http_server/uri.js

// https://tc39.github.io/ecma262/#sec-array.prototype.find
if (!Array.prototype.find) {
  Object.defineProperty(Array.prototype, 'find', {
    value: function(predicate) {
     // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return kValue.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return kValue;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return undefined.
      return undefined;
    },
    configurable: true,
    writable: true
  });
}

// https://tc39.github.io/ecma262/#sec-array.prototype.findindex
if (!Array.prototype.findIndex) {
  Object.defineProperty(Array.prototype, 'findIndex', {
    value: function(predicate) {
     // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return k.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return k;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return -1.
      return -1;
    },
    configurable: true,
    writable: true
  });
}

// Point
Point.prototype.x = null;
Point.prototype.y = null;

function Point(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
}

// CompatLayer
var CompatLayer =
{
	is_internet_explorer : !!navigator.userAgent.match(/Trident/g) || !!navigator.userAgent.match(/MSIE/g),
	
	is_firefox : !!navigator.userAgent.match(/Firefox/g),
	
	is_chrome : !!navigator.userAgent.match(/Chrome/g) && !!navigator.vendor.match(/Google Inc/g),
	
	has_svg : !!(typeof SVGRect !== undefined),

	get_element_width : function(el)
	{
		var style = getComputedStyle(el, null);
		var width = parseFloat(style.getPropertyValue('width'));
		if(this.is_internet_explorer)
		{
			var border_left_width = parseFloat(style.getPropertyValue('border-left-width'));
			var border_right_width = parseFloat(style.getPropertyValue('border-right-width'));
			var padding_left = parseFloat(style.getPropertyValue('padding-left'));
			var padding_right = parseFloat(style.getPropertyValue('padding-right'));
			
			return width + border_left_width + border_right_width + padding_left + padding_right;
		}
		
		return width;
	},

	get_element_height : function(el)
	{
		var style = getComputedStyle(el, null);
		var height = parseFloat(style.getPropertyValue('height'));
		if(this.is_internet_explorer)
		{
			var border_top_width = parseFloat(style.getPropertyValue('border-top-width'));
			var border_bottom_width = parseFloat(style.getPropertyValue('border-bottom-width'));
			var padding_top = parseFloat(style.getPropertyValue('padding-top'));
			var padding_bottom = parseFloat(style.getPropertyValue('padding-bottom'));
			
			return height + border_top_width + border_bottom_width + padding_top + padding_bottom;
		}
		
		return height;
	},
	
	get_element_pos : function(el)
	{
	    var rect = el.getBoundingClientRect();
	    var scroll_left = window.pageXOffset || document.documentElement.scrollLeft;
	    var scroll_top = window.pageYOffset || document.documentElement.scrollTop;
	    return new Point(rect.left + scroll_left, rect.top + scroll_top);
	},

	get_iframe_scroll_pos : function(iframe)
	{
		return this.is_internet_explorer ? new Point(iframe.contentDocument ? iframe.contentDocument.documentElement.scrollLeft : 0, iframe.contentDocument ? iframe.contentDocument.documentElement.scrollTop : 0)
		                                 : new Point(iframe.contentWindow ? iframe.contentWindow.scrollX : 0, iframe.contentWindow ? iframe.contentWindow.scrollY : 0);
	},

	set_iframe_scroll_pos : function(iframe, scroll_pos)
	{
		if(this.is_internet_explorer)
		{
			if(iframe.contentDocument) iframe.contentDocument.documentElement.scrollLeft = scroll_pos.x;
			if(iframe.contentDocument) iframe.contentDocument.documentElement.scrollTop = scroll_pos.y;
		}
		else
		{
			if(iframe.contentWindow) iframe.contentWindow.scrollTo(scroll_pos.x, scroll_pos.y);
		}
	},
	
	get_iframe_content_height : function(iframe)
	{
		return Math.max(iframe.contentDocument.documentElement.scrollHeight,
		                iframe.contentDocument.documentElement.offsetHeight,
                        iframe.contentDocument.documentElement.clientHeight);
	},
	
	scroll_iframe_to_bottom : function(iframe)
	{
		CompatLayer.set_iframe_scroll_pos(iframe, new Point(CompatLayer.get_iframe_scroll_pos(iframe).x, CompatLayer.get_iframe_content_height(iframe) - CompatLayer.get_element_height(iframe) - 4));
	}
}

URI.prototype.custom_compare = function(uri)
{
	if(!this.compare(uri))
	{
		// no strictly equal
		if((this.scheme != uri.scheme) || (this.authority != uri.authority) || (this.path != uri.path)) return false; // scheme, authority or path differ
		
		if(!this.fragment || !uri.fragment || (this.fragment != uri.fragment)) return false; // fragments differ

		// try to match key=value in queries excluding key foo for the match
		var own_map = new Map();
		var map = new Map();
			
		if(this.query)
		{
			var own_assignments = this.query.split('&');
			for(var i = 0; i < own_assignments.length; i++)
			{
				var own_assignment = own_assignments[i].split('=');
				if(own_assignment.length == 2)
				{
					own_map[own_assignment[0].trim()] = own_assignment[1].trim();
				}
			}
		}

		if(uri.query)
		{
			var assignments = uri.query.split('&');
			for(var i = 0; i < assignments.length; i++)
			{
				var assignment = assignments[i].split('=');
				if(assignment.length == 2)
				{
					map[assignment[0].trim()] = assignment[1].trim();
				}
			}
		}
		
		// this.query <= uri.query ?
		for(var key in own_map)
		{
			if(key != 'foo')
			{
				if(!map.hasOwnProperty(key)) return false;
				if(this.map[key] != map[key]) return false;
			}
		}

		// uri.query <= this.query ?
		for(var key in map)
		{
			if(key != 'foo')
			{
				if(!map.hasOwnProperty(key)) return false;
				if(this.map[key] != map[key]) return false;
			}
		}
	}

	return true;
}

function rewrite_uri_with_fragment(uri)
{
	// if URI has a fragment, we rewrite it so that it has a query with an argument foo
	// which value is incremented so that browser is forced to reload content from server:
	//   /path/to/resource#label             => /path/to/resource?foo=0#label
	//   /path/to/resource?foo=5&bar=z#label => /path/to/resource?foo=6&bar=z#label

	// rebuild uri adding/updating foo=x
	var new_uri = new URI(uri);
	
	if(new_uri.fragment)
	{
		if(new_uri.query)
		{
			var assignments = new_uri.query.split('&');
			new_uri.query = '';
			
			var found_foo = false;
			var i;
			for(i = 0; i < assignments.length; i++)
			{
				if(i != 0)
				{
					new_uri.query += '&';
				}
				
				var assignment_str = assignments[i];
				
				var assignment = assignment_str.split('=');
				
				if((assignment.length == 2) && (assignment[0].trim() == 'foo'))
				{
					found_foo = true;
					
					var foo = parseInt(assignment[1].trim(), 10);
					if(!isNaN(foo))
					{
						new_uri.query += 'foo=' + (foo + 1);
					}
					else
					{
						new_uri.query += 'foo=0';
					}
				}
				else
				{
					new_uri.query += assignment_str;
				}
			}
			
			if(!found_foo)
			{
				if(assignments.length != 0)
				{
					new_uri.query += '&';
				}
				new_uri.query += 'foo=0';
			}
		}
		else
		{
			// URI has no query: add a query foo=0
			new_uri.query = 'foo=0';
		}
	}
	
	return new_uri;
}

// IFrame
IFrame.prototype.owner = null;
IFrame.prototype.index = 0;
IFrame.prototype.iframe_element = null;
IFrame.prototype.load_requested = false;
IFrame.prototype.cancelled_load = false;
IFrame.prototype.loaded = false;
IFrame.prototype.bound_on_load = null;
IFrame.prototype.bound_on_mouseenter = null;
IFrame.prototype.bound_on_mouseleave = null;

function IFrame(owner,index)
{
	this.owner = owner;
	this.index = index;
	this.iframe_element = document.createElement('iframe');
	this.iframe_element.className = this.owner.class_name;
	this.iframe_element.setAttribute('name', this.owner.name + '-' + index);
	this.iframe_element.setAttribute('id', this.owner.name + '-' + index);
	this.iframe_element.setAttribute('src', this.owner.uri);
	this.load_requested = false; //true; // load is fired twice: on iframe creation, on assignment of src attribute
	this.load_cancelled = false;
	this.loaded = false;
	this.bound_on_load = this.owner.on_load.bind(owner, index);
	this.iframe_element.addEventListener('load', this.bound_on_load);
	if(CompatLayer.is_firefox)
	{
		this.bound_on_mouseenter = this.owner.on_mouseenter.bind(owner);
		this.iframe_element.addEventListener('mouseenter', this.bound_on_mouseenter);
		this.bound_on_mouseleave = this.owner.on_mouseleave.bind(owner);
		this.iframe_element.addEventListener('mouseleave', this.bound_on_mouseleave);
	}
	else
	{
		this.bound_on_mouseenter = null;
		this.bound_on_mouseleave = null;
		//this.iframe_element.setAttribute('scrolling', 'yes');
		this.iframe_element.style.overflow = 'auto';
	}
}

IFrame.prototype.destroy = function()
{
	this.iframe_element.removeEventListener('load', this.bound_on_load);
	if(this.bound_on_mouseenter) this.iframe_element.removeEventListener('mouseenter', this.bound_on_mouseenter);
	if(this.bound_on_mouseleave) this.iframe_element.removeEventListener('mouseleave', this.bound_on_mouseleave);
	this.iframe_element = null;
	this.load_requested = false;
	this.load_cancelled = false;
	this.loaded = false;
}

// DoubleIFrame
DoubleIFrame.prototype.owner = null;
DoubleIFrame.prototype.name = null;
DoubleIFrame.prototype.uri = null;
DoubleIFrame.prototype.class_name = null;
DoubleIFrame.prototype.storage_item_prefix = null;
DoubleIFrame.prototype.parent_element = null;
DoubleIFrame.prototype.visible = false;
DoubleIFrame.prototype.bg = 0;
DoubleIFrame.prototype.fg = 1;
DoubleIFrame.prototype.iframes = null;
DoubleIFrame.prototype.bound_on_mouseup = null;

function DoubleIFrame(owner, name, uri, class_name, storage_item_prefix, parent_element)
{
	this.owner = owner;
	this.name = name;
	this.uri = uri;
	this.class_name = class_name;
	this.storage_item_prefix = storage_item_prefix;
	this.visible = false;
	this.bg = 0;
	this.fg = 1;
	this.iframes = new Array(2);
	for(var i = 0; i < 2; i++)
	{
		this.iframes[i] = new IFrame(this, i);
	}
	this.bound_on_mouseup = null;
	this.attach(parent_element);
}

DoubleIFrame.prototype.destroy = function()
{
	this.detach();
	for(var i = 0; i < 2; i++)
	{
		this.iframes[i].destroy();
		this.iframes[i] = null;
	}
	this.name = null;
	this.uri = null;
	this.class_name = null;
	this.storage_item_prefix = null;
	this.parent_element = null;
}

DoubleIFrame.prototype.on_load = function(i)
{
// 	console.log(this.name + ':frame #' + i + ' loaded');
	var iframe = this.iframes[i];
	if(!iframe.iframe_element.hasAttribute('src'))
	{
// 		console.log(this.name + ':frame #' + i + ' has no src attribute');
		// Initially our iframe elements have no src attribute;
		// A load event may be triggered after iframe creation with a blank content, like if src='#blank': ignore such load event
		return;
	}
	if(iframe.load_cancelled)
	{
// 		console.log(this.name + ':frame #' + i + ' load cancelled');
		iframe.loaded = false;
		iframe.load_requested = false;
		iframe.load_cancelled = false;
		return;
	}

// 	console.log(this.name + ':frame #' + i + ' will show');
	
	// save scroll position of foreground iframe if loaded
	this.save_scroll_position();
	this.bg = i; // last loaded iframe is the background iframe at the moment
	this.fg = 1 - this.bg;
	
	var bg_iframe = this.iframes[this.bg];
	var fg_iframe = this.iframes[this.fg];
	if(!bg_iframe.load_requested && fg_iframe.load_requested)
	{
		// this is not a load we've requested but we've already requested a load on another iframe: user may have click an hyperlink in the iframe, so cancel our refresh in other iframe
		fg_iframe.load_cancelled = true;
	}
	var uri = null;
	if(!bg_iframe.load_requested)
	{
		// this is not a load we've requested: uri may have changed
		try
		{
			uri = new URI(bg_iframe.iframe_element.contentWindow.location.href);
		}
		catch(e)
		{
			uri = new URI(bg_iframe.iframe_element.getAttribute('src'));
		}
	}
	bg_iframe.loaded = true; // mark background iframe as loaded
	bg_iframe.load_requested = false;
	bg_iframe.load_cancelled = false;
	// update tab uri (it may have changed because of user click on hyperlink within iframe of tab)
	if(uri && !uri.compare(this.uri))
	{
		this.uri = uri;
		this.owner.tab_config.uri = uri;
		this.owner.owner.save_tabs();
		if(fg_iframe.load_requested) fg_iframe.load_cancelled = true; // cancel foreground iframe if there's a pending because URI has changed
	}

	this.restore_scroll_position(); // restore scroll position for background iframe
	
	this.patch_document();
	// simulate unloading before flipping
	this.trigger_on_unload();
	this.owner.unfreeze();
	var title = bg_iframe.iframe_element.contentDocument ? bg_iframe.iframe_element.contentDocument.title : null;
	this.owner.set_label(title ? title : 'Untitled');
	// flipping: show freshly loaded iframe
	bg_iframe.iframe_element.setAttribute('style', 'visibility:' + (this.visible ? 'visible' : 'hidden'));
	// flipping: hide old iframe
	if(fg_iframe.iframe_element) fg_iframe.iframe_element.setAttribute('style', 'visibility:hidden');
	// flipping: swap iframe role (background/foreground)
	this.fg = this.bg;
	this.bg = 1 - this.fg;
	// simulate loading after flipping
	this.trigger_on_load();
// 	console.log(this.name + ':next frame is frame #' + this.bg);
}

DoubleIFrame.prototype.on_mouseup = function(element)
{
// 	console.log(this.owner.tab_config.name + ' mouseup');
	this.owner.unfreeze();
	element.removeEventListener('mouseup', this.bound_on_mouseup);
	this.bound_on_mouseup = null;
}

DoubleIFrame.prototype.patch_document = function()
{
	// patch anchors within iframe to provide user with a context menu for each anchor
	this.patch_anchors();
	// patch inputs within iframe to freeze/unfreeze when focus in/focus out input
	this.patch_inputs('input');
	this.patch_inputs('select');
	// patch inputs within iframe to freeze/unfreeze when mouse down/up button
	this.patch_buttons();
}

DoubleIFrame.prototype.patch_anchors = function()
{
	var bg_iframe = this.iframes[this.bg];
	var iframe_element = bg_iframe.iframe_element;
	
	if(iframe_element.contentDocument)
	{
		var anchor_elements = iframe_element.contentDocument.documentElement.getElementsByTagName('a');
		for(var i = 0; i < anchor_elements.length; i++)
		{
			var anchor_element = anchor_elements[i];
			
			anchor_element.addEventListener('mousedown',
				function(event)
				{
					this.owner.freeze();
					this.bound_on_mouseup = this.on_mouseup.bind(this, anchor_element);
					iframe_element.contentDocument.addEventListener('mouseup', this.bound_on_mouseup);
				}.bind(this)
			);
			
			if(anchor_element.hasAttribute('href'))
			{
				var href = anchor_element.getAttribute('href');
				var uri = new URI(href, iframe_element.contentWindow);
	// 			console.log('on_dom_content_loaded:\'' + href + '\' => \'' + uri.toString() + '\'');
				var uri_str = uri.toString();
				anchor_element.addEventListener('contextmenu',
					function(uri_str, event)
					{
						this.owner.freeze();
						var pos = CompatLayer.get_element_pos(iframe_element);
						pos.x += event.clientX;
						pos.y += event.clientY;
						var context_menu_items = new Array();
						context_menu_items.push(
							{
								label:'Open in new tab',
								action:function() { gui.open_tab(this.owner.owner.name, this.owner.tab_config.name_seed, uri_str); },
								arg:uri_str
							}
						);
						
						var tiles = {
							left_tile       : { name : 'left-tile',       label: 'left' },
							top_middle_tile : { name : 'top-middle-tile', label: 'center' },
							top_right_tile  : { name : 'top-right-tile',  label: 'right' },
							bottom          : { name : 'bottom-tile',     label: 'bottom' }
						};
						
						for(var tile in tiles)
						{
							var tile_name = tiles[tile].name;
							var tile_label = tiles[tile].label;
							if(this.owner.owner.name != tile_name)
							{
								context_menu_items.push(
									{
										label:'Open to the ' + tile_label,
										action:function(tile_name) { gui.open_tab(tile_name, this.owner.tab_config.name_seed, uri_str); },
										arg: tile_name
									}
								);
							}
						}
						
						gui.create_context_menu(
							pos.x,
							pos.y,
							context_menu_items,
							this,
							function()
							{
								this.owner.unfreeze();
							}
						);
						event.preventDefault();
					}.bind(this, uri_str)
				);
			}
		}
	}
}

DoubleIFrame.prototype.patch_inputs = function(tag_name)
{
	var bg_iframe = this.iframes[this.bg];
	var iframe_element = bg_iframe.iframe_element;
	
	if(iframe_element.contentDocument)
	{
		var input_elements = iframe_element.contentDocument.documentElement.getElementsByTagName(tag_name);
		for(var i = 0; i < input_elements.length; i++)
		{
			var input_element = input_elements[i];
			input_element.addEventListener('focusin',
				function(event)
				{
					this.owner.freeze();
				}.bind(this)
			);
			input_element.addEventListener('focusout',
				function(event)
				{
					this.owner.unfreeze();
				}.bind(this)
			);
		}
	}
}

DoubleIFrame.prototype.patch_buttons = function()
{
	var bg_iframe = this.iframes[this.bg];
	var iframe_element = bg_iframe.iframe_element;
	
	if(iframe_element.contentDocument)
	{
		var input_elements = iframe_element.contentDocument.documentElement.getElementsByTagName('button');
		for(var i = 0; i < input_elements.length; i++)
		{
			var input_element = input_elements[i];
			input_element.addEventListener('mousedown',
				function(event)
				{
					this.owner.freeze();
					this.bound_on_mouseup = this.on_mouseup.bind(this, input_element);
					iframe_element.contentDocument.addEventListener('mouseup', this.bound_on_mouseup);
				}.bind(this)
			);
		}
	}
}

DoubleIFrame.prototype.update_uri = function()
{
	if(this.uri.fragment)
	{
		var new_uri = rewrite_uri_with_fragment(this.uri);
		
		this.uri = new_uri;
		this.owner.tab_config.uri = new_uri;
		this.owner.owner.save_tabs();
	}

	if(!this.uri.compare(new URI(window.location.href)))
	{
		var bg_iframe = this.iframes[this.bg];
		// update src attribute of iframe
		if(bg_iframe.iframe_element.hasAttribute('src'))
		{
			bg_iframe.iframe_element.contentWindow.location.replace(this.uri.toString());
		}
		else
		{
			bg_iframe.iframe_element.setAttribute('src', this.uri.toString());
		}
	}
	else
	{
		window.alert("Error! recursive iframe problem detected");
	}
}

DoubleIFrame.prototype.refresh = function()
{
// 	console.log(this.name + ':refresh');
	var bg_iframe = this.iframes[this.bg];
	bg_iframe.loaded = false;
	bg_iframe.load_requested = true;
	bg_iframe.load_cancelled = false;
	this.update_uri();
}

DoubleIFrame.prototype.trigger_on_unload = function()
{
	var fg_iframe = this.iframes[this.fg];
	if(fg_iframe.loaded)
	{
		var fg_iframe_element = fg_iframe.iframe_element;
		if(fg_iframe_element.contentWindow && fg_iframe_element.contentWindow.gui && (fg_iframe_element.contentWindow.gui.magic == 0xC0CA))
		{
			fg_iframe_element.contentWindow.gui.__on_unload__();
		}
	}
}

DoubleIFrame.prototype.trigger_on_load = function()
{
	var fg_iframe = this.iframes[this.fg];
	if(fg_iframe.loaded)
	{
		var fg_iframe_element = fg_iframe.iframe_element;
		if(fg_iframe_element.contentWindow && fg_iframe_element.contentWindow.gui && (fg_iframe_element.contentWindow.gui.magic == 0xC0CA))
		{
			fg_iframe_element.contentWindow.gui.__on_load__();
		}
	}
}

DoubleIFrame.prototype.set_visible = function(v)
{
	this.visible = v;
	var fg_iframe = this.iframes[this.fg];
	fg_iframe.iframe_element.style.visibility = (this.visible && fg_iframe.loaded) ? 'visible' : 'hidden';
}

DoubleIFrame.prototype.on_mouseenter = function()
{
	for(var i = 0; i < 2; i++)
	{
		var iframe = this.iframes[i];
		iframe.iframe_element.setAttribute('scrolling', 'yes');
		iframe.iframe_element.style.overflow = 'auto';
	}
}

DoubleIFrame.prototype.on_mouseleave = function()
{
	for(var i = 0; i < 2; i++)
	{
		var iframe = this.iframes[i];
		iframe.iframe_element.setAttribute('scrolling', 'no');
		iframe.iframe_element.style.overflow = 'hidden';
	}
}

DoubleIFrame.prototype.save_scroll_position = function()
{
	var fg_iframe = this.iframes[this.fg];
	if(!fg_iframe.loaded) return;
	
	if(!fg_iframe.iframe_element.contentWindow) return;
	
	if(!fg_iframe.iframe_element.contentDocument) return;
	
	var scroll_x_item_name = this.storage_item_prefix + 'scroll-x';
	var scroll_y_item_name = this.storage_item_prefix + 'scroll-y';
	
	var scroll_pos = CompatLayer.get_iframe_scroll_pos(fg_iframe.iframe_element);
	
// 	console.log(this.name + ':' + scroll_x_item_name + ' <- ' + scroll_pos.x);
// 	console.log(this.name + ':' + scroll_y_item_name + ' <- ' + scroll_pos.y);
	
	sessionStorage.setItem(scroll_x_item_name, scroll_pos.x);
	sessionStorage.setItem(scroll_y_item_name, scroll_pos.y);
}

DoubleIFrame.prototype.restore_scroll_position = function()
{
	var bg_iframe = this.iframes[this.bg];
	if(!bg_iframe.loaded) return;
	
	if(!bg_iframe.iframe_element.contentWindow) return;
	
	if(!bg_iframe.iframe_element.contentDocument) return;

	var scroll_x_item_name = this.storage_item_prefix + 'scroll-x';
	var scroll_y_item_name = this.storage_item_prefix + 'scroll-y';
	
	var scroll_x = sessionStorage.getItem(scroll_x_item_name);
	var scroll_y = sessionStorage.getItem(scroll_y_item_name);
	
	if((scroll_x !== null) && (scroll_y !== null))
	{
// 		console.log(this.name + ':' + scroll_x_item_name + ' == ' + scroll_x);
// 		console.log(this.name + ':' + scroll_y_item_name + ' == ' + scroll_y);
		
		CompatLayer.set_iframe_scroll_pos(bg_iframe.iframe_element, new Point(scroll_x, scroll_y));
	}
}

DoubleIFrame.prototype.set_scroll_pos = function(scroll_pos)
{
	for(var i = 0; i < 2; i++)
	{
		var iframe = this.iframes[i];
		CompatLayer.set_iframe_scroll_pos(iframe.iframe_element, scroll_pos);
	}
}

DoubleIFrame.prototype.has_iframe = function(iframe_name)
{
	for(var i = 0; i < 2; i++)
	{
		var iframe = this.iframes[i];
		if(iframe.iframe_element.hasAttribute('name') && (iframe.iframe_element.getAttribute('name') == iframe_name)) return true;
	}
	
	return false;
}

DoubleIFrame.prototype.is_foreground_iframe = function(iframe_name)
{
	var fg_iframe = this.iframes[this.fg];
	return fg_iframe.iframe_element.hasAttribute('name') && (fg_iframe.iframe_element.getAttribute('name') == iframe_name);
}

DoubleIFrame.prototype.get_next_target = function(iframe_name)
{
	if(this.iframes[0].iframe_element.hasAttribute('name') && this.iframes[1].iframe_element.hasAttribute('name'))
	{
		if(this.iframes[0].iframe_element.getAttribute('name') == iframe_name)
		{
			return this.iframes[1].iframe_element.getAttribute('name');
		}
		else if(this.iframes[1].iframe_element.getAttribute('name') == iframe_name)
		{
			return this.iframes[0].iframe_element.getAttribute('name');
		}
	}
	
	return '_self';
}

DoubleIFrame.prototype.detach = function()
{
	this.save_scroll_position();
	
	if(this.parent_element)
	{
		for(var i = 0; i < 2; i++)
		{
			var iframe = this.iframes[i];
			this.parent_element.removeChild(iframe.iframe_element);
			iframe.loaded = false;
		}
		this.parent_element = null;
	}
}

DoubleIFrame.prototype.attach = function(parent_element)
{
	this.parent_element = parent_element;
	for(var i = 0; i < 2; i++)
	{
		var iframe = this.iframes[i];
		this.parent_element.appendChild(iframe.iframe_element);
	}
	this.restore_scroll_position();
}

DoubleIFrame.prototype.get_foreground_iframe = function()
{
	return this.iframes[this.fg];
}

DoubleIFrame.prototype.get_background_iframe = function()
{
	return this.iframes[this.bg];
}

DoubleIFrame.prototype.scroll_to_bottom = function()
{
	CompatLayer.scroll_iframe_to_bottom(this.iframes[this.fg].iframe_element);
}

// ChildWindow
ChildWindow.prototype.wnd = null;
ChildWindow.prototype.refresh_called = false;

function ChildWindow(wnd)
{
	this.wnd = wnd;
	this.refresh_called = false;
}

ChildWindow.prototype.refresh = function()
{
	var href = rewrite_uri_with_fragment(this.wnd.location.href);
	this.wnd.location.replace(href); // reload without POST
}

ChildWindow.prototype.closed = function()
{
	return this.wnd.closed;
}

// StatusBarItem
StatusBarItem.prototype.name = null;
StatusBarItem.prototype.div_element = null;

function StatusBarItem(div_element)
{
	this.name = div_element.hasAttribute('name') ? div_element.getAttribute('name') : null;
	this.div_element = div_element;
}

// TabConfig
TabConfig.prototype.name_seed = null;
TabConfig.prototype.name = null;
TabConfig.prototype.uri = null;
TabConfig.prototype.label = null;

function TabConfig(name_seed, name, uri)
{
	this.name_seed = name_seed;
	this.name = name;
	this.uri = uri;
	this.label = null;
}

TabConfig.prototype.save = function(prefix)
{
	if(this.name_seed) sessionStorage.setItem(prefix + 'name_seed', this.name_seed);
	if(this.name) sessionStorage.setItem(prefix + 'name', this.name);
	if(this.uri) sessionStorage.setItem(prefix + 'uri', this.uri.toString());
	if(this.label) sessionStorage.setItem(prefix + 'label', this.label);
}

TabConfig.prototype.restore = function(prefix)
{
	this.name_seed = sessionStorage.getItem(prefix + 'name_seed');
	this.name = sessionStorage.getItem(prefix + 'name');
	this.uri = new URI(sessionStorage.getItem(prefix + 'uri'));
	this.label = sessionStorage.getItem(prefix + 'label');
}

// TabConfigs
TabConfigs.prototype.tab_configs = null;

function TabConfigs()
{
	this.tab_configs = new Array();
}

TabConfigs.prototype.push = function(tab_config)
{
	if(!this.find_tab_config(tab_config))
	{
		this.tab_configs.push(tab_config);
	}
}

TabConfigs.prototype.remove = function(tab_config)
{
	var tab_config_index = this.tab_configs.findIndex(function(other_tab_config) { return other_tab_config == tab_config; });
	if(tab_config_index >= 0)
	{
		this.tab_configs.splice(tab_config_index, 1);
	}
}

TabConfigs.prototype.save = function(prefix)
{
	sessionStorage.setItem(prefix + 'length', this.tab_configs.length);
	this.tab_configs.forEach(function(tab_config, index) { tab_config.save(prefix + 'tab' + index + '.'); });
}

TabConfigs.prototype.restore = function(prefix)
{
	var length = sessionStorage.getItem(prefix + 'length') || 0;
	for(var i = 0; i < length; i++)
	{
		var tab_config = new TabConfig();
		tab_config.restore(prefix + 'tab' + i + '.');
		if(!this.find_tab_config_by_name(tab_config.name))
		{
			this.tab_configs.push(tab_config);
		}
	}
}

TabConfigs.prototype.find_tab_config = function(tab_config)
{
	return this.tab_configs.find(function(other_tab_config) { return other_tab_config == tab_config; });
}

TabConfigs.prototype.find_tab_config_by_name = function(name)
{
	return this.tab_configs.find(function(tab_config) { return tab_config.name == name; });
}

TabConfigs.prototype.find_tab_config_by_uri = function(uri)
{
	return this.tab_configs.find(function(tab_config) { return tab_config.uri && tab_config.uri.custom_compare(uri); });
}

// Tab
Tab.prototype.owner = null;
Tab.prototype.tab_config = null;
Tab.prototype.static = false;
Tab.prototype.tab_header = null;
Tab.prototype.tab_content = null;
Tab.prototype.tab_header_label = null;
Tab.prototype.close_tab = null;
Tab.prototype.bound_close_tab_onclick = null;
Tab.prototype.is_active = false;
Tab.prototype.bound_tab_header_onclick = null;
Tab.prototype.bound_tab_header_oncontextmenu = null;
Tab.prototype.storage_item_prefix = null;
Tab.prototype.refresh_mode = null; // - 'self-refresh': tab is periodically refreshed periodically regardless of whether is active or not
                                   // - 'self-refresh-when-active': tab is periodically refreshed periodically if it's active
                                   // - 'global': all tabs and child windows are refreshed periodically regardless of whether tab that initiate the refresh is active or not
                                   // - 'global-refresh-when-active': all tabs and child windows are refreshed periodically if tab that initiate the refresh is active
Tab.prototype.refresh_period = 0;
Tab.prototype.refresh_called = false;
Tab.prototype.frozen = false;

function Tab(owner)
{
	this.owner = owner;
	this.tab_config = null;
	this.static = false;
	this.tab_header = null;
	this.tab_content = null;
	this.tab_header_label = null;
	this.close_tab = null;
	this.bound_close_tab_onclick = null;
	this.is_active = false;
	this.bound_tab_header_onclick = null;
	this.bound_tab_header_oncontextmenu = null;
	this.storage_item_prefix = null;
	this.refresh_mode = 'self-refresh-when-active';
	this.refresh_period = 0;
	this.refresh_called = false;
	this.frozen = false;
}

Tab.prototype.create = function(tab_config)
{
	this.static = false;
	this.tab_config = tab_config;
	this.close_tab = document.createElement('div');
	this.close_tab.className = 'close-tab ' + (CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.close_tab.setAttribute('title', 'Close');
	this.tab_header = document.createElement('div');
	this.tab_header.className = 'tab-header noselect';
	this.tab_header_label = document.createElement('span');
	this.tab_header_label.innerHTML = 'Loading&hellip;';
	this.tab_header.appendChild(this.tab_header_label);
	this.tab_header.appendChild(this.close_tab);
	this.owner.tab_headers.appendChild(this.tab_header);
	this.storage_item_prefix = 'gui.' + this.owner.name + '.' + this.tab_config.name + '.';
	this.tab_content = new DoubleIFrame(this, this.tab_config.name, this.tab_config.uri, 'tab-content', this.storage_item_prefix, this.owner.tab_contents);
	
	this.bound_tab_header_onclick = this.tab_header_onclick.bind(this);
	this.tab_header.addEventListener('click', this.bound_tab_header_onclick, false);
	
	this.bound_tab_header_oncontextmenu = this.tab_header_oncontextmenu.bind(this);
	this.tab_header.addEventListener('contextmenu', this.bound_tab_header_oncontextmenu);
	
	this.bound_close_tab_onclick = this.close_tab_onclick.bind(this);
	this.close_tab.addEventListener('click', this.bound_close_tab_onclick);
}

Tab.prototype.reuse_static = function(tab_header, tab_content)
{
	this.static = true;
	this.tab_config = new TabConfig();
	this.tab_header = tab_header;
	this.tab_content = tab_content;
	if(this.tab_content.hasAttribute('name'))
	{
		var name = this.tab_content.getAttribute('name');
		this.tab_config.name = name;
	}
	else if(this.tab_content.hasAttribute('id'));
	{
		var id = this.tab_content.getAttribute('id');
		this.tab_config.name = id;
	}
	this.bound_tab_header_onclick = this.tab_header_onclick.bind(this);
	this.tab_header.addEventListener('click', this.bound_tab_header_onclick, false);

	this.close_tab = null;
	this.bound_close_tab_onclick = null;
}

Tab.prototype.destroy = function()
{
	if(!this.static)
	{
		this.refresh_period = 0;
		this.owner.owner.auto_reload_tab(this);
		
		if(this.owner)
		{
			this.owner.tab_headers.removeChild(this.tab_header);
		}
		this.tab_content.destroy();
		this.tab_content = null;
		this.tab_header.removeEventListener('click', this.bound_tab_header_onclick);
		if(this.close_tab)
		{
			this.close_tab.removeEventListener('click', this.bound_close_tab_onclick);
		}
		this.owner = null;
		this.tab_config = null;
		this.tab_header = null;
		this.tab_content = null;
		this.tab_header_label = null;
		this.close_tab = null;
		this.bound_close_tab_onclick = null;
		this.bound_tab_header_onclick = null;
	}
}

Tab.prototype.detach = function()
{
	this.owner.detach_tab(this);
}

Tab.prototype.attach = function(to)
{
	to.attach_tab(this);
}

Tab.prototype.save = function(prefix)
{
	this.tab_config.save(prefix);
}

Tab.prototype.restore = function(prefix)
{
	this.tab_config.restore(prefix);
}

Tab.prototype.refresh = function()
{
	if(!this.static)
	{
		this.tab_content.refresh();
	}
}

Tab.prototype.auto_reload = function(period, mode)
{
	this.refresh_mode = mode;
	this.refresh_period = period;
	this.owner.owner.auto_reload_tab(this);
}

Tab.prototype.save_scroll_position = function()
{
	if(!this.static)
	{
		this.tab_content.save_scroll_position();
	}
}

Tab.prototype.restore_scroll_position = function()
{
	if(!this.static)
	{
		this.tab_content.restore_scroll_position();
	}
}

Tab.prototype.set_active = function(v)
{
	if(this.is_active != v)
	{
		if(v)
		{
			this.tab_header.classList.add('tab-header-active');
		}
		else
		{
			this.tab_header.classList.remove('tab-header-active');
		}
		if(this.static)
		{
			this.tab_content.style.visibility = v ? 'visible' : 'hidden';
		}
		else
		{
			this.tab_content.set_visible(v);
		}
		this.is_active = v;
	}
}

Tab.prototype.switch_to = function()
{
	this.owner.switch_to(this);
	this.refresh();
}

Tab.prototype.tab_header_onclick = function(event)
{
	this.switch_to();
}

Tab.prototype.tab_header_oncontextmenu = function(event)
{
	var context_menu_items = [
	{
		label : 'Close',
		action : function()
		{
			this.close();
		}
	},
	{
		label : 'Close all',
		action : function()
		{
			this.owner.close_all_tabs();
		}
	},
	{
		label : 'Move to new window',
		action : function()
		{
			gui.move_tab_to_new_window(this);
		}
	} ];
	
	if(this.owner.name != 'left-tile')
	{
		context_menu_items.push(
		{
			label : 'Move to left',
			action : function()
			{
				gui.move_tab_to_tile(this, 'left-tile');
			}
		});
	}
	
	if(this.owner.name != 'top-middle-tile')
	{
		context_menu_items.push(
		{
			label : 'Move to center',
			action : function()
			{
				gui.move_tab_to_tile(this, 'top-middle-tile');
			}
		});
	}
	
	if(this.owner.name != 'top-right-tile')
	{
		context_menu_items.push(
		{
			label : 'Move to right',
			action : function()
			{
				gui.move_tab_to_tile(this, 'top-right-tile');
			}
		});
	}

	if(this.owner.name != 'bottom-tile')
	{
		context_menu_items.push(
		{
			label : 'Move to bottom',
			action : function()
			{
				gui.move_tab_to_tile(this, 'bottom-tile');
			}
		});
	}

	event.preventDefault();
	this.owner.owner.create_context_menu(event.clientX, event.clientY, context_menu_items, this);
}

Tab.prototype.close_tab_onclick = function(event)
{
	this.close();
	event.stopImmediatePropagation(); // avoid calling of this.tab_header_onclick after tab destruction
}

Tab.prototype.close = function()
{
	if(!this.static)
	{
		this.owner.destroy_tab(this);
	}
}

Tab.prototype.has_iframe = function(iframe_name)
{
	return !this.static && this.tab_content.has_iframe(iframe_name);
}

Tab.prototype.is_foreground_iframe = function(iframe_name)
{
	return !this.static && this.tab_content.is_foreground_iframe(iframe_name);
}

Tab.prototype.get_next_target = function(iframe_name)
{
	return this.static ? '_self' : this.tab_content.get_next_target(iframe_name);
}

Tab.prototype.get_foreground_iframe = function()
{
	return this.static ? null : this.tab_content.get_foreground_iframe();
}

Tab.prototype.get_background_iframe = function()
{
	return this.static ? null : this.tab_content.get_background_iframe();
}

Tab.prototype.set_label = function(label)
{
	this.tab_config.label = label;
	this.tab_header_label.textContent = label;
	this.tab_header_label.setAttribute('title', label + '\n\nTips:\n- Click on tab to switch to it\n- Right click to show a context menu\n- Click on the cross to close the tab');
}

Tab.prototype.get_pos = function()
{
	return this.owner.get_tab_contents_pos();
}

Tab.prototype.freeze = function()
{
// 	console.log(this.tab_config.name + ':freeze()');
	this.frozen = true;
}

Tab.prototype.unfreeze = function()
{
// 	console.log(this.tab_config.name + ':unfreeze()');
	this.frozen = false;
}

Tab.prototype.scroll_to_bottom = function()
{
	if(!this.static)
	{
		this.tab_content.scroll_to_bottom();
	}
}

// Tile
Tile.prototype.owner = null;
Tile.prototype.name = null;
Tile.prototype.tile_element = null;
Tile.prototype.tab_headers = null;
Tile.prototype.tab_contents = null;
Tile.prototype.tab_headers_left_scroller = null;
Tile.prototype.tab_headers_right_scroller = null;
Tile.prototype.tabs = null;
Tile.prototype.tile_width = 0;
Tile.prototype.controls_width = 0;
Tile.prototype.tab_headers_width = 0;
Tile.prototype.tab_config_history = null;
Tile.prototype.history_shortcut_element = null;

function Tile(owner, name, tile_element)
{
	this.owner = owner;
	this.name = name;
	this.tile_element = tile_element;
	this.tab_headers = this.tile_element.getElementsByClassName('tab-headers')[0];
	this.tab_contents = this.tile_element.getElementsByClassName('tab-contents')[0];
	this.history_shortcut_element = this.tile_element.getElementsByClassName('tab-headers-history-shortcut')[0];
	this.history_shortcut_element.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tab_headers_left_scroller = this.tile_element.getElementsByClassName('tab-headers-left-scroller')[0];
	this.tab_headers_left_scroller.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tab_headers_right_scroller = this.tile_element.getElementsByClassName('tab-headers-right-scroller')[0];
	this.tab_headers_right_scroller.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tabs = new Array();
	this.tab_config_history = new TabConfigs();
	var saved_tab_configs = new TabConfigs();
	saved_tab_configs.restore('gui.' + this.name + '.');
	
	var history_shortcut_element_width = CompatLayer.get_element_width(this.history_shortcut_element);
	var left_scroller_width = CompatLayer.get_element_width(this.tab_headers_left_scroller);
	var right_scroller_width = CompatLayer.get_element_width(this.tab_headers_right_scroller);
	this.controls_width = history_shortcut_element_width + left_scroller_width + right_scroller_width;
	
	// reuse static tabs
	var static_tab_headers = this.tab_headers.getElementsByClassName('tab-header');
	var static_tab_contents = this.tab_contents.getElementsByClassName('tab-content');
	
	for(var i = 0; (i < static_tab_headers.length) && (i < static_tab_contents.length); i++)
	{
		var tab = new Tab(this);
		var tab_header = static_tab_headers[i];
		var tab_content = static_tab_contents[i];
		tab.reuse_static(tab_header, tab_content);
		var saved_tab_config = saved_tab_configs.find_tab_config_by_name(tab.tab_config.name);
		if(saved_tab_config)
		{
			tab.tab_config = saved_tab_config;
			saved_tab_configs.remove(saved_tab_config);
		}
		if(!this.find_tab_by_name(tab.tab_config.name))
		{
			this.tabs.push(tab);
			this.save_tabs();
		}
	}
	
	// recreate tabs from saved tab configs
	saved_tab_configs.tab_configs.forEach(function(saved_tab_config) { this.create_tab_from_config(saved_tab_config); }, this);
	
	this.compute_dynamic_widths();
	if(this.tab_headers_left_scroller) this.tab_headers_left_scroller.addEventListener('click', this.scroll_tab_headers.bind(this, -32));
	if(this.tab_headers_right_scroller) this.tab_headers_right_scroller.addEventListener('click', this.scroll_tab_headers.bind(this, +32));
	
	if(this.history_shortcut_element) this.history_shortcut_element.addEventListener('click', this.history_shortcut_onclick.bind(this));
	
	this.restore_history();
	this.restore_active_tab();
	this.restore_tab_headers_left();
	this.tabs.forEach(function(tab) { if(!tab.is_active) tab.refresh(); });
}

Tile.prototype.has_tabs = function()
{
	return this.tabs.length != 0;
}

Tile.prototype.save_active_tab = function(tab)
{
	sessionStorage.setItem('gui.' + this.name + '.active-tab', tab.tab_config.name);
}

Tile.prototype.save_tabs = function()
{
	sessionStorage.setItem('gui.' + this.name + '.length', this.tabs.length);
	for(var i = 0; i < this.tabs.length; i++)
	{
		var tab = this.tabs[i];
		tab.save('gui.' + this.name + '.tab' + i + '.');
	}
}

Tile.prototype.save_history = function()
{
	this.tab_config_history.save('gui.' + this.name + '.history' + '.');
}

Tile.prototype.save_tab_headers_left = function()
{
	sessionStorage.setItem('gui.' + this.name + '.tab-headers.style.left', this.tab_headers.style.left);
}

Tile.prototype.restore_active_tab = function()
{
	var name = sessionStorage.getItem('gui.' + this.name + '.active-tab');
	var tab = this.find_tab_by_name(name);
	if(tab)
	{
		tab.switch_to();
	}
}

Tile.prototype.restore_history = function()
{
	this.tab_config_history.restore('gui.' + this.name + '.history' + '.');
}

Tile.prototype.restore_tab_headers_left = function()
{
	var tab_headers_left = sessionStorage.getItem('gui.' + this.name + '.tab-headers.style.left');
	if(tab_headers_left) this.tab_headers.style.left = tab_headers_left;
}

Tile.prototype.compute_dynamic_widths = function()
{
	this.tile_width = CompatLayer.get_element_width(this.tile_element);
	this.tab_headers_width = 0;
	var tab_headers = this.tab_headers.getElementsByClassName('tab-header');
	for(var i = 0; i < tab_headers.length; i++)
	{
		var tab_header = tab_headers[i];
		var tab_header_width = CompatLayer.get_element_width(tab_header);
		this.tab_headers_width += tab_header_width;
	}
}

Tile.prototype.find_tab_config_history_by_name = function(name)
{
	return this.tab_config_history.find_tab_config_by_name(name);
}

Tile.prototype.create_tab = function(name_seed, name, uri)
{
	tab = new Tab(this);
	var tab_config = this.tab_config_history.find_tab_config_by_uri(uri);
	if(tab_config)
	{
		this.tab_config_history.remove(tab_config);
		this.save_history();
	}
	else
	{
		tab_config = new TabConfig(name_seed, name, uri)
	}
	tab.create(tab_config);
	this.tabs.push(tab);
	this.save_tabs();
	return tab;
}

Tile.prototype.create_tab_from_config = function(tab_config)
{
	var tab = this.find_tab_by_name(tab_config.name);
	
	if(!tab)
	{
		tab = new Tab(this);
		tab.create(tab_config);
		this.tabs.push(tab);
		this.save_tabs();
	}
	return tab;
}

Tile.prototype.destroy_tab = function(tab_to_destroy)
{
	var tab_num = this.tabs.findIndex(function(tab) { return tab == tab_to_destroy; });
	
	if(tab_num >= 0)
	{
		this.tab_config_history.push(tab_to_destroy.tab_config);
		var is_active = tab_to_destroy.is_active;
		this.tabs.splice(tab_num, 1);
		tab_to_destroy.destroy();
		this.save_tabs();
		this.save_history();
		if(is_active && (this.tabs.length != 0))
		{
			this.switch_to_nth_tab((tab_num > 0) ? (tab_num - 1) : tab_num);
		}
	}
}

Tile.prototype.destroy_nth_tab = function(tab_num)
{
	this.destroy_tab(this.tabs[tab_num]);
}

Tile.prototype.switch_to = function(tab)
{
	for(var tab_num = 0; tab_num < this.tabs.length; tab_num++)
	{
		var is_active = (this.tabs[tab_num] == tab);
		this.tabs[tab_num].set_active(is_active);
		if(is_active)
		{
			this.ensure_tab_header_is_in_visible_scope(tab);
			this.save_active_tab(tab);
		}
	}
}

Tile.prototype.switch_to_nth_tab = function(tab_num)
{
	if(tab_num < this.tabs.length)
	{
		var tab = this.tabs[tab_num];
		tab.switch_to();
	}
}

Tile.prototype.find_tab = function(tab)
{
	return this.tabs.find(function(elt) { return elt == tab; });
}

Tile.prototype.find_tab_by_name = function(name)
{
	return this.tabs.find(function(tab) { return tab.tab_config.name == name; });
}

Tile.prototype.find_tab_by_iframe_name = function(iframe_name)
{
	return this.tabs.find(function(elt) { return elt.has_iframe(iframe_name); });
}

Tile.prototype.find_tab_by_uri = function(uri)
{
	return this.tabs.find(function(tab) { return tab.tab_config.uri && tab.tab_config.uri.custom_compare(uri); });
}

Tile.prototype.scroll_tab_headers = function(dx)
{
	this.compute_dynamic_widths();
	var left = parseFloat(getComputedStyle(this.tab_headers, '').left);
	left = left + dx;
	left = Math.max(left, this.tile_width - this.tab_headers_width);
	left = Math.min(this.controls_width, left);
	this.tab_headers.style.left = left + 'px';
	this.save_tab_headers_left();
}

Tile.prototype.ensure_tab_header_is_in_visible_scope = function(tab)
{
	this.compute_dynamic_widths();
	var tile_element_pos = CompatLayer.get_element_pos(this.tile_element);
	var tab_header_pos = CompatLayer.get_element_pos(tab.tab_header);
	var tab_headers_pos = CompatLayer.get_element_pos(this.tab_headers);
	var tile_width = CompatLayer.get_element_width(this.tile_element);
	var tab_header_width = CompatLayer.get_element_width(tab.tab_header);
	var tab_headers_left = 0;
	var out_of_scope_by_left = (tab_header_pos.x < (tile_element_pos.x + this.controls_width));
	var out_of_scope_by_right = ((tab_header_pos.x + tab_header_width) > (tile_element_pos.x + tile_width));
	var fix_tab_headers_left = false;

	if(tab_header_width < (tile_width - this.controls_width))
	{
		if(out_of_scope_by_left)
		{
			// out of visible scope by the left
			var dx = tab_headers_pos.x - tab_header_pos.x;
			tab_header_pos.x = tile_element_pos.x + this.controls_width; 
			tab_headers_left = tab_header_pos.x - tile_element_pos.x + dx;
			fix_tab_headers_left = true;
		}
		else if(out_of_scope_by_right)
		{
			// out of visible scope by the right
			var dx = tab_headers_pos.x - tab_header_pos.x;
			tab_header_pos.x = tile_element_pos.x + tile_width - tab_header_width;
			tab_headers_left = tab_header_pos.x - tile_element_pos.x + dx;
			fix_tab_headers_left = true;
		}
	}

	if(fix_tab_headers_left)
	{
		this.tab_headers.style.left = tab_headers_left + 'px';
		this.save_tab_headers_left();
	}
}

Tile.prototype.history_shortcut_onclick = function(event)
{
	if(this.tab_config_history.tab_configs.length > 0)
	{
		// create an 'history' below the 'history shortcut'
		var history_shortcut_element_pos = CompatLayer.get_element_pos(this.history_shortcut_element);
		var x = history_shortcut_element_pos.x;
		var y = history_shortcut_element_pos.y + CompatLayer.get_element_height(this.history_shortcut_element);
		var history_items = new Array();
		for(var i = 0; i < this.tab_config_history.tab_configs.length; i++)
		{
			var tab_config = this.tab_config_history.tab_configs[i];
			
			history_items.push(
				{
					label: tab_config.label,
					action:function(tab_config)
					{
						this.create_tab_from_config(tab_config).switch_to();
						this.tab_config_history.remove(tab_config);
						this.save_history();
					},
					arg: tab_config
				}
			);
		}
		
		event.stopPropagation();
		this.owner.create_context_menu(x, y, history_items, this);
	}
}

Tile.prototype.detach_tab = function(tab_to_detach)
{
	var tab_num = this.tabs.findIndex(function(tab) { return tab == tab_to_detach; });
	
	if(tab_num >= 0)
	{
		var is_active = tab_to_detach.is_active;
		this.tabs.splice(tab_num, 1);
		this.save_tabs();
		this.save_history();
		if(is_active && (this.tabs.length != 0))
		{
			this.switch_to_nth_tab((tab_num > 0) ? (tab_num - 1) : tab_num);
		}
		this.tab_headers.removeChild(tab_to_detach.tab_header);
		if(this.static)
		{
			this.tab_contents.removeChild(tab_to_detach.tab_content);
		}
		else
		{
			tab_to_detach.tab_content.detach();
		}
		tab_to_detach.owner = null;
	}
}

Tile.prototype.attach_tab = function(tab)
{
	if(tab.owner != this)
	{
		tab.owner = this;
		this.tab_headers.appendChild(tab.tab_header);
		if(this.static)
		{
			this.tab_contents.appendChild(tab.tab_content);
		}
		else
		{
			tab.tab_content.attach(this.tab_contents);
		}
		this.tabs.push(tab);
		this.save_tabs();
	}
}

Tile.prototype.for_each_tab = function(callback, callback_this)
{
	this.tabs.forEach(callback, callback_this);
}

Tile.prototype.close_all_tabs = function()
{
	while(this.tabs.length)
	{
		this.tabs[0].close();
	}
}

Tile.prototype.get_tab_contents_pos = function()
{
	return CompatLayer.get_element_pos(this.tab_contents);
}

// ContextMenu
ContextMenu.action_this = null;
ContextMenu.on_destroy_action = null;
ContextMenu.context_menu_element = null;

function ContextMenu(x, y, context_menu_items, action_this, on_destroy_action)
{
	this.action_this = action_this;
	this.on_destroy_action = on_destroy_action;
	this.context_menu_element = document.createElement('div');
	this.context_menu_element.className = 'context-menu';
	this.context_menu_element.style.display = 'block';
	this.context_menu_element.style.position = 'fixed';
	this.context_menu_element.style.left = x + 'px';
	this.context_menu_element.style.top = y + 'px';
	
	for(var i = 0; i < context_menu_items.length; i++)
	{
		var context_menu_item_element = document.createElement('div');
		context_menu_item_element.className = 'context-menu-item';
		context_menu_item_element.textContent = context_menu_items[i].label;
		if(this.action_this)
		{
			if(context_menu_items[i].arg)
			{
				context_menu_item_element.addEventListener('click', context_menu_items[i].action.bind(this.action_this, context_menu_items[i].arg));
			}
			else
			{
				context_menu_item_element.addEventListener('click', context_menu_items[i].action.bind(this.action_this));
			}
		}
		else
		{
			if(context_menu_items[i].arg)
			{
				context_menu_item_element.addEventListener('click', context_menu_items[i].action.bind(context_menu_item_element, context_menu_items[i].arg));
			}
			else
			{
				context_menu_item_element.addEventListener('click', context_menu_items[i].action);
			}
//			context_menu_item_element.addEventListener('click', context_menu_items[i].action);
		}
		this.context_menu_element.appendChild(context_menu_item_element);
	}
	
	document.body.appendChild(this.context_menu_element);
	
	var window_inner_width = window.innerWidth;
	var window_inner_height = window.innerHeight;
	var style = getComputedStyle(this.context_menu_element, null);
	var left = parseFloat(style.getPropertyValue('left'));
	var top = parseFloat(style.getPropertyValue('top'));
	var width = parseFloat(style.getPropertyValue('width'));
	var height = parseFloat(style.getPropertyValue('height'));
	if((left + width) >= window_inner_width)
	{
		this.context_menu_element.style.left = (window_inner_width - width) + 'px';
	}
	if((top + height) >= window_inner_height)
	{
		this.context_menu_element.style.top = (window_inner_height - height) + 'px';
	}
}

ContextMenu.prototype.destroy = function()
{
	if(this.on_destroy_action)
	{
		this.on_destroy_action.call(this.action_this);
	}
	document.body.removeChild(this.context_menu_element);
	this.action_this = null;
	this.context_menu_element = null;
}

// RefreshEvent
RefreshEvent.prototype.time_stamp = null;
RefreshEvent.prototype.tab = null;

function RefreshEvent(time_stamp, tab)
{
	this.time_stamp = time_stamp;
	this.tab = tab;
}

// RefreshScheduler
RefreshScheduler.prototype.owner = null;
RefreshScheduler.prototype.time_stamp = 0;
RefreshScheduler.prototype.time_resolution = null; // in milliseconds
RefreshScheduler.prototype.new_time_resolution = null; 
RefreshScheduler.prototype.refresh_events = null;
RefreshScheduler.prototype.tabs = null;
RefreshScheduler.prototype.interval_id = null;

function RefreshScheduler(owner)
{
	this.owner = owner;
	this.time_stamp = 0;
	this.time_resolution = 100;
	this.new_time_resolution = null;
	this.refresh_events = new Array();
	this.tabs = new Array();
	this.interval_id = window.setInterval(this.schedule.bind(this), this.time_resolution);
}

RefreshScheduler.prototype.set_time_resolution = function(time_resolution)
{
	this.new_time_resolution = time_resolution;
}

RefreshScheduler.prototype.schedule = function()
{
	this.time_stamp += this.time_resolution;
	if(this.refresh_events.length)
	{
		this.owner.for_each_tab(
			function(tab)
			{
				tab.refresh_called = false;
			}
		);
		
		this.owner.for_each_child_window(
			function(child_window)
			{
				child_window.refresh_called = false;
			}
		);
		
		var refresh_event = this.refresh_events[0];
		if(this.time_stamp >= refresh_event.time_stamp)
		{
			do
			{
				this.refresh_events.splice(0, 1);
				
				var tab = refresh_event.tab;
				if(tab.frozen)
				{
// 					console.log(this.time_stamp + ' ms:' + tab.tab_config.name + ' is frozen');
					this.refresh_tab_after(tab);
				}
				else
				{
// 					console.log(this.time_stamp + ' ms:' + tab.refresh_mode + ' refresh from ' + tab.tab_config.name);
					switch(tab.refresh_mode)
					{
						case 'self-refresh':
						case 'self-refresh-when-active':
							if(((tab.refresh_mode != 'self-refresh-when-active') || tab.is_active) && !tab.frozen && !tab.refresh_called)
							{
								this.refresh_tab(tab);
							}
							break;
							
						case 'global-refresh':
						case 'global-refresh-when-active':
							if((tab.refresh_mode != 'global-refresh-when-active') || tab.is_active)
							{
								// refresh tab
								if(!tab.frozen && !tab.refresh_called)
								{
									this.refresh_tab(tab);
								}
								
								// refresh other tabs that are active
								this.owner.for_each_tab(
									function(other_tab)
									{
										if(!other_tab.frozen && !other_tab.refresh_called && other_tab.is_active && (other_tab != tab) && !other_tab.refresh_period)
										{
											this.refresh_tab(other_tab);
										}
									}, this
								);
								
								// refresh child windows
								this.owner.for_each_child_window(
									function(child_window)
									{
										if(!child_window.refresh_called)
										{
											child_window.refresh();
											child_window.refresh_called = true;
										}
									}
								);
							}
							break;
					}
				}
				
				refresh_event = this.refresh_events.length ? this.refresh_events[0] : null;
			}
			while(refresh_event && (this.time_stamp >= refresh_event.time_stamp));
		}
	}
	if(this.new_time_resolution && (this.time_resolution != this.new_time_resolution))
	{
		this.time_resolution = this.new_time_resolution;
		this.new_time_resolution = null;
		if(this.interval_id)
		{
			window.clearInterval(this.interval_id);
		}
		this.interval_id = window.setInterval(this.bind.schedule(this), this.time_resolution);
	}
}

RefreshScheduler.prototype.refresh_tab = function(tab)
{
	if(!tab.refresh_called)
	{
		tab.refresh();
		tab.refresh_called = true;
		if(tab.refresh_period)
		{
			this.auto_reload_tab(tab);
		}
	}
}

RefreshScheduler.prototype.refresh_tab_after = function(tab)
{
	var delay = tab.refresh_period;
	var skew = delay % this.time_resolution;
	if(skew) delay += this.time_resolution - skew;
	var time_stamp = this.time_stamp + delay;
	if(this.refresh_events.length)
	{
		for(var i = 0; i < this.refresh_events.length; i++)
		{
			var refresh_event = this.refresh_events[i];
			if(((refresh_event.tab.refresh_mode == 'global-refresh') || (refresh_event.tab.refresh_mode == 'global-refresh-when-active')) &&
			   Math.abs(refresh_event.time_stamp - time_stamp <= delay))
			{
				time_stamp = refresh_event.time_stamp;
				break;
			}
		}
		for(var i = 0; i < this.refresh_events.length; i++)
		{
			var refresh_event = this.refresh_events[i];
			if((time_stamp != refresh_event.time_stamp) && (refresh_event.tab != tab) && (time_stamp < refresh_event.time_stamp))
			{
				this.refresh_events.splice(i, 0, new RefreshEvent(time_stamp, tab));
				return;
			}
		}
	}
	
	this.refresh_events.push(new RefreshEvent(time_stamp, tab));
}

RefreshScheduler.prototype.auto_reload_tab = function(tab)
{
	if(tab.refresh_period)
	{
		for(var i = 0; i < this.refresh_events.length; i++)
		{
			var refresh_event = this.refresh_events[i];
			if(refresh_event.tab == tab) return; // already scheduled
		}
		
		this.refresh_tab_after(tab);
	}
	else
	{
		// deschedule
		var i = 0;
		while(i < this.refresh_events.length)
		{
			var refresh_event = this.refresh_events[i];
			if(refresh_event.tab == tab)
			{
				this.refresh_events.splice(i, 1);
			}
			else
			{
				i++;
			}
		}
	}
}

// GUI
// +---------------------------------------------------------------------------------------------------------------------------------------+
// |                                                   toolbar-div                                                                         |
// +---------------------------------------------------------------------------------------------------------------------------------------+
// +---------------------------------------------------------------------------------------------------------------------------------------+
// | content-div                                                                                                                           |
// |  +-------------------------------+  +-+  +------------------------------------------------------------------------------------------+ |
// |  | left-tile-div                 |  | |  | right-div                                                                                | |
// |  | +---------------------------+ |  | |  | +--------------------------------------------------------------------------------------+ | |
// |  | | left-tab-headers-div      | |  | |  | | top-div                                                                              | | |
// |  | | +-----------------------+ | |  | |  | | +-------------------------------------+  +-+  +------------------------------------+ | | |
// |  | | | left-tab-header-div*  | | |  | |  | | | top-middle-tile-div                 |  |r|  | top-right-tile-div                 | | | |
// |  | | |                       | | |  | |  | | | +---------------------------------+ |  |i|  | +--------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | top-middle-tab-headers-div      | |  |g|  | | top-right-tab-headers-div      | | | | |
// |  | | |                       | | |  | |  | | | | +-----------------------------+ | |  |h|  | | +----------------------------+ | | | | |
// |  | | |                       | | |  | |  | | | | | top-middle-tab-header-div*  | | |  |t|  | | | top-right-tab-header-div*  | | | | | |
// |  | | |                       | | |  | |  | | | | |                             | | |  |-|  | | |                            | | | | | |
// |  | | +-----------------------+ | |  | |  | | | | +-----------------------------+ | |  |h|  | | +----------------------------+ | | | | |
// |  | +---------------------------+ |  | |  | | | +---------------------------------+ |  |o|  | +--------------------------------+ | | | |
// |  | +---------------------------+ |  | |  | | | +---------------------------------+ |  |r|  | +--------------------------------+ | | | |
// |  | | left-tab-contents-div     | |  |l|  | | | | top-middle-tab-contents-div     | |  |i|  | |   top-right-tab-contents-div   | | | | |
// |  | | +-----------------------+ | |  |e|  | | | | +-----------------------------+ | |  |z|  | | +----------------------------+ | | | | |
// |  | | | left-tab-content-div* | | |  |f|  | | | | | top-middle-tab-content-div* | | |  |-|  | | | top-right-tab-content-div* | | | | | |
// |  | | |                       | | |  |t|  | | | | |                             | | |  |r|  | | |                            | | | | | |
// |  | | |                       | | |  |-|  | | | | |                             | | |  |e|  | | |                            | | | | | |
// |  | | |                       | | |  |h|  | | | | |                             | | |  |s|  | | |                            | | | | | |
// |  | | |                       | | |  |o|  | | | | |                             | | |  |i|  | | |                            | | | | | |
// |  | | |                       | | |  |r|  | | | | |                             | | |  |z|  | | |                            | | | | | |
// |  | | |                       | | |  |i|  | | | | |                             | | |  |e|  | | |                            | | | | | |
// |  | | |                       | | |  |z|  | | | | |                             | | |  |r|  | | |                            | | | | | |
// |  | | |                       | | |  |-|  | | | | |                             | | |  |-|  | | |                            | | | | | |
// |  | | |                       | | |  |r|  | | | | |                             | | |  |d|  | | |                            | | | | | |
// |  | | |                       | | |  |e|  | | | | +-----------------------------+ | |  |i|  | | +----------------------------+ | | | | |
// |  | | |                       | | |  |s|  | | | +---------------------------------+ |  |v|  | +--------------------------------+ | | | |
// |  | | |                       | | |  |i|  | | +-------------------------------------+  +-+  +------------------------------------+ | | |
// |  | | |                       | | |  |z|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |e|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |r|  | |                              vert-resizer-div                                        | | |
// |  | | |                       | | |  |-|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |d|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |i|  | | bottom-tile-div                                                                      | | |
// |  | | |                       | | |  |v|  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | | bottom-tab-headers-div                                                           | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | bottom-tab-header-div*                                                       | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | | bottom-tab-contents-div                                                          | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | bottom-tab-content-div*                                                      | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | +-----------------------+ | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | +---------------------------+ |  | |  | +--------------------------------------------------------------------------------------+ | |
// |  +-------------------------------+  +-+  +------------------------------------------------------------------------------------------+ |
// +---------------------------------------------------------------------------------------------------------------------------------------+

GUI.prototype.magic = 0xCAFE;
GUI.prototype.min_tab_content_width = 48;  // minimum tab content width: enough to display tab header history and scrollers
GUI.prototype.min_tab_content_height = 0; // minimum tab content width
GUI.prototype.window_size_monitoring_period = 500; // interface refresh period (in ms) while window is being resized
GUI.prototype.toolbar_div = null;
GUI.prototype.content_div = null;
GUI.prototype.statusbar_div = null;
GUI.prototype.left_tile_div = null;
GUI.prototype.left_tab_headers_div = null;
GUI.prototype.left_tab_header_div = null;
GUI.prototype.left_tab_contents_div = null;
GUI.prototype.left_tab_content_div = null;
GUI.prototype.left_horiz_resizer_div = null;
GUI.prototype.right_div = null;
GUI.prototype.top_div = null;
GUI.prototype.top_middle_tile_div = null;
GUI.prototype.top_middle_tab_headers_div = null;
GUI.prototype.top_middle_tab_header_div = null;
GUI.prototype.top_middle_tab_contents_div = null;
GUI.prototype.right_horiz_resizer_div = null;
GUI.prototype.top_right_tile_div = null;
GUI.prototype.top_right_tab_headers_div = null;
GUI.prototype.top_right_tab_header_div = null;
GUI.prototype.top_right_tab_contents_div = null;
GUI.prototype.vert_resizer_div = null;
GUI.prototype.bottom_tile_div = null;
GUI.prototype.bottom_tab_headers_div = null;
GUI.prototype.bottom_tab_header_div = null;
GUI.prototype.bottom_tab_contents_div = null;
GUI.prototype.left_tile = null;
GUI.prototype.top_middle_tile = null;
GUI.prototype.top_right_tile = null;
GUI.prototype.bottom_tile = null;
GUI.prototype.statusbar_items = null;
GUI.prototype.overlay = null; // an overlay div to prevent iframes from capturing mouse events while a menu is opened
GUI.prototype.mouse_pos = null;
GUI.prototype.bound_vert_resize = null;
GUI.prototype.bound_left_horiz_resize = null;
GUI.prototype.bound_right_horiz_resize = null;
GUI.prototype.window_inner_width = 0;
GUI.prototype.window_inner_height = 0;
GUI.prototype.context_menu = null;
GUI.prototype.child_windows_monitoring_period = 500;
GUI.prototype.child_windows = null;
GUI.prototype.refresh_scheduler = null;

function GUI()
{
	if(window.frameElement && window.parent.gui && window.parent.gui.magic == this.magic)
	{
		window.alert("Warning! GUI should not be run within itself");
	}
	
	var magic = sessionStorage.getItem('gui.magic');
	if(!magic || (magic != this.magic))
	{
		sessionStorage.clear();
	}
	
	this.toolbar_div = document.getElementById('toolbar-div');
	this.content_div = document.getElementById('content-div');
	this.statusbar_div = document.getElementById('statusbar-div');
	this.left_tile_div = document.getElementById('left-tile-div');
	this.left_tab_headers_div = document.getElementById('left-tab-headers-div');
	this.left_tab_header_div = document.getElementsByClassName('left-tab-header-div');
	this.left_tab_contents_div = document.getElementById('left-tab-contents-div');
	this.left_tab_content_div = document.getElementsByClassName('left-tab-content-div');
	this.left_horiz_resizer_div = document.getElementById('left-horiz-resizer-div');
	this.right_div = document.getElementById('right-div');
	this.top_div = document.getElementById('top-div');
	this.top_middle_tile_div = document.getElementById('top-middle-tile-div');
	this.top_middle_tab_headers_div = document.getElementById('top-middle-tab-headers-div');
	this.top_middle_tab_header_div = document.getElementsByClassName('top-middle-tab-header-div');
	this.top_middle_tab_contents_div = document.getElementById('top-middle-tab-contents-div');
	this.right_horiz_resizer_div = document.getElementById('right-horiz-resizer-div');
	this.top_right_tile_div = document.getElementById('top-right-tile-div');
	this.top_right_tab_headers_div = document.getElementById('top-right-tab-headers-div');
	this.top_right_tab_header_div = document.getElementsByClassName('top-right-tab-header-div');
	this.top_right_tab_contents_div = document.getElementById('top-right-tab-contents-div');
	this.vert_resizer_div = document.getElementById('vert-resizer-div');
	this.bottom_tile_div = document.getElementById('bottom-tile-div');
	this.bottom_tab_headers_div = document.getElementById('bottom-tab-headers-div');
	this.bottom_tab_header_div = document.getElementsByClassName('bottom-tab-header-div');
	this.bottom_tab_contents_div = document.getElementById('bottom-tab-contents-div');
	this.statusbar_items = new Array();
	var statusbar_item_elements = document.getElementsByClassName('statusbar-item');
	for(var i = 0; i < statusbar_item_elements.length; i++)
	{
		var statusbar_item_element = statusbar_item_elements[i];
		this.statusbar_items.push(new StatusBarItem(statusbar_item_element));
	}
	this.overlay = document.createElement('div');
	this.overlay.setAttribute('id', 'overlay');
	this.overlay.style.zIndex = 2;
	this.overlay.style.position = 'fixed';
	this.overlay.style.left = 0 + 'px';
	this.overlay.style.right = 0 + 'px';
	this.overlay.style.display = 'none';
	document.body.appendChild(this.overlay);
	this.mouse_pos = new Point(0, 0);
	this.bound_vert_resize = null;
	this.bound_left_horiz_resize = null;
	this.bound_right_horiz_resize = null;
	this.window_inner_width = 0;
	this.window_inner_height = 0;
	this.context_menu = null;
	this.child_windows = new Array();
	
	this.refresh_scheduler = new RefreshScheduler(this);

	this.left_tile = new Tile(this, 'left-tile', this.left_tile_div);
	this.top_middle_tile = new Tile(this, 'top-middle-tile', this.top_middle_tile_div);
	this.top_right_tile = new Tile(this, 'top-right-tile', this.top_right_tile_div);
	this.bottom_tile = new Tile(this, 'bottom-tile', this.bottom_tile_div);

	this.vert_resizer_div.addEventListener('mousedown', this.vert_resizer_on_mousedown.bind(this));
	this.left_horiz_resizer_div.addEventListener('mousedown', this.left_horiz_resizer_on_mousedown.bind(this));
	this.right_horiz_resizer_div.addEventListener('mousedown', this.right_horiz_resizer_on_mousedown.bind(this));

	document.addEventListener('mouseup', this.document_on_mouseup.bind(this));
	document.addEventListener('click', function(event) { if(event.button == 0) this.destroy_context_menu(); }.bind(this));
	
	this.restore_layout();
	this.window_inner_width = window.innerWidth;
	this.window_inner_height = window.innerHeight;
	this.resize();
	
	setInterval(this.monitor_window_size.bind(this), this.window_size_monitoring_period);
	
	if(!magic || (magic != this.magic))
	{
		this.left_tile.switch_to_nth_tab(0);
	}
	
	sessionStorage.setItem('gui.magic', this.magic);
	
	setInterval(this.monitor_child_windows.bind(this), this.child_windows_monitoring_period);
}

GUI.prototype.open_tab = function(tile_name, name, uri_str, dont_switch_to)
{
	var uri = new URI(uri_str);
// 	console.log('open_tab: \'' + uri_str + '\' => \'' + uri.toString());
	switch(tile_name)
	{
		case 'left-tile':
			tab = this.left_tile.find_tab_by_uri(uri);
			break;
		case 'top-middle-tile':
			tab = this.top_middle_tile.find_tab_by_uri(uri);
			break;
		case 'top-right-tile':
			tab = this.top_right_tile.find_tab_by_uri(uri);
			break;
		case 'bottom-tile':
			tab = this.bottom_tile.find_tab_by_uri(uri);
			break;
	}
	
	if(!tab)
	{
		switch(tile_name)
		{
			case 'left-tile':
				if(!this.left_tile.has_tabs()) dont_switch_to = false;
				tab = this.left_tile.create_tab(name, this.gen_unique_tab_name(name), uri);
				break;
			case 'top-middle-tile':
				if(!this.top_middle_tile.has_tabs()) dont_switch_to = false;
				tab = this.top_middle_tile.create_tab(name, this.gen_unique_tab_name(name), uri);
				break;
			case 'top-right-tile':
				if(!this.top_right_tile.has_tabs()) dont_switch_to = false;
				tab = this.top_right_tile.create_tab(name, this.gen_unique_tab_name(name), uri);
				break;
			case 'bottom-tile':
				if(!this.bottom_tile.has_tabs()) dont_switch_to = false;
				tab = this.bottom_tile.create_tab(name, this.gen_unique_tab_name(name), uri);
				break;
		}
	}
	
	if(tab)
	{
		if(dont_switch_to)
		{
			tab.refresh();
		}
		else
		{
			tab.switch_to();
		}
	}
	
	return tab;
}

GUI.prototype.set_cursor = function(cursor)
{
	this.overlay.style.cursor = cursor;
}

GUI.prototype.enable_events = function(v)
{
	if((v === undefined) || v)
	{
		this.overlay.style.display = 'none';
	}
	else
	{
		this.overlay.style.display = 'block';
	}
}

GUI.prototype.vert_resizer_on_mousedown = function(e)
{
	this.mouse_pos.y = window.devicePixelRatio ? Math.round(e.screenY / window.devicePixelRatio) : e.screenY;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_events(false);
	this.set_cursor('row-resize');
	if(this.bound_vert_resize == null)
	{
		this.bound_vert_resize = this.vert_resize.bind(this);
		document.addEventListener('mousemove', this.bound_vert_resize);
	}
}

GUI.prototype.left_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_events(false);
	this.set_cursor('col-resize');
	if(this.bound_left_horiz_resize == null)
	{
		this.bound_left_horiz_resize = this.left_horiz_resize.bind(this);
		document.addEventListener('mousemove', this.bound_left_horiz_resize);
	}
}

GUI.prototype.right_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_events(false);
	this.set_cursor('col-resize');
	if(this.bound_right_horiz_resize == null)
	{
		this.bound_right_horiz_resize = this.right_horiz_resize.bind(this);
		document.addEventListener('mousemove', this.bound_right_horiz_resize);
	}
}

GUI.prototype.document_on_mouseup = function(e)
{
	if(this.bound_vert_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_vert_resize);
		this.content_div.classList.remove('noselect');
		this.set_cursor('auto');
		this.enable_events();
		this.bound_vert_resize = null;
	}
	if(this.bound_left_horiz_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_left_horiz_resize);
		this.content_div.classList.remove('noselect');
		this.set_cursor('auto');
		this.enable_events();
		this.bound_left_horiz_resize = null;
	}
	if(this.bound_right_horiz_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_right_horiz_resize);
		this.content_div.classList.remove('noselect');
		this.set_cursor('auto');
		this.enable_events();
		this.bound_right_horiz_resize = null;
	}
}

GUI.prototype.vert_resize = function(e)
{
	var y = window.devicePixelRatio ? Math.round(e.screenY / window.devicePixelRatio) : e.screenY;
	var dy = y - this.mouse_pos.y;
	this.mouse_pos.y = y;
	
	var top_middle_tab_headers_div_height = CompatLayer.get_element_height(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = CompatLayer.get_element_height(this.top_right_tab_headers_div);
	var bottom_tab_headers_div_height = CompatLayer.get_element_height(this.bottom_tab_headers_div);
	
	var top_div_height = CompatLayer.get_element_height(this.top_div);
	var bottom_tile_div_height = CompatLayer.get_element_height(this.bottom_tile_div);
	
	top_div_height = top_div_height + dy;
	bottom_tile_div_height = bottom_tile_div_height - dy;
	var top_middle_tab_contents_div_height = top_div_height - top_middle_tab_headers_div_height;
	var top_right_tab_contents_div_height = top_div_height - top_right_tab_headers_div_height;
	var bottom_tab_contents_div_height = bottom_tile_div_height - bottom_tab_headers_div_height;
	
	if((top_middle_tab_contents_div_height >= this.min_tab_content_height) &&
	   (top_right_tab_contents_div_height >= this.min_tab_content_height) &&
	   (bottom_tab_contents_div_height >= this.min_tab_content_height))
	{
		this.top_div.style.height = top_div_height + 'px';
		this.top_middle_tile_div.style.height = top_div_height + 'px';
		this.top_middle_tab_contents_div.style.height = top_middle_tab_contents_div_height + 'px';
		this.right_horiz_resizer_div.style.height = top_div_height + 'px';
		this.top_right_tile_div.style.height = top_div_height + 'px';
		this.top_right_tab_contents_div.style.height = top_right_tab_contents_div_height + 'px';
		this.bottom_tile_div.style.height = bottom_tile_div_height + 'px';
		this.bottom_tab_contents_div.style.height = bottom_tab_contents_div_height + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.left_horiz_resize = function(e)
{
	var x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	var dx = x - this.mouse_pos.x;
	this.mouse_pos.x = x;
	
	var left_tile_div_width = CompatLayer.get_element_width(this.left_tile_div);
	var right_div_width = CompatLayer.get_element_width(this.right_div);
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	
	left_tile_div_width = left_tile_div_width + dx;
	right_div_width = right_div_width - dx;
	top_middle_tile_div_width = top_middle_tile_div_width - dx;
	
	if((left_tile_div_width >= this.min_tab_content_width) && (right_div_width >= this.min_tab_content_width) && (top_middle_tile_div_width >= this.min_tab_content_width))
	{
		this.left_tile_div.style.width = left_tile_div_width + 'px';
		this.left_tab_contents_div.style.width = left_tile_div_width + 'px';
		this.right_div.style.width = right_div_width + 'px';
		this.top_div.style.width = right_div_width + 'px';
		this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
		this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
		this.vert_resizer_div.style.width = right_div_width + 'px';
		this.bottom_tile_div.style.width = right_div_width + 'px';
		this.bottom_tab_contents_div.style.width = right_div_width + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.right_horiz_resize = function(e)
{
	var x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	var dx = x - this.mouse_pos.x;
	this.mouse_pos.x = x;
	
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	var top_right_tile_div_width = CompatLayer.get_element_width(this.top_right_tile_div);
	
	top_middle_tile_div_width = top_middle_tile_div_width + dx;
	top_right_tile_div_width = top_right_tile_div_width - dx;
	
	if((top_middle_tile_div_width >= this.min_tab_content_width) && (top_right_tile_div_width >= this.min_tab_content_width))
	{
		this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
		this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
		this.top_right_tile_div.style.width = top_right_tile_div_width + 'px';
		this.top_right_tab_contents_div.style.width = top_right_tile_div_width + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.save_layout = function()
{
	sessionStorage.setItem('gui.left-tile-div.style.width', this.left_tile_div.style.width);
	sessionStorage.setItem('gui.right-div.style.width', this.right_div.style.width);
	sessionStorage.setItem('gui.top-middle-tile-div.style.width', this.top_middle_tile_div.style.width);
	sessionStorage.setItem('gui.top-right-tile-div.style.width', this.top_right_tile_div.style.width);
	sessionStorage.setItem('gui.bottom-tile-div.style.width', this.bottom_tile_div.style.width);
	sessionStorage.setItem('gui.top-div.style.height', this.top_div.style.height);
	sessionStorage.setItem('gui.bottom-tile-div.style.height', this.bottom_tile_div.style.height);
}

GUI.prototype.restore_layout = function()
{
	var left_tile_div_style_width = sessionStorage.getItem('gui.left-tile-div.style.width');
	var right_div_style_width = sessionStorage.getItem('gui.right-div.style.width');
	var top_middle_tile_div_style_width = sessionStorage.getItem('gui.top-middle-tile-div.style.width');
	var top_right_tile_div_style_width = sessionStorage.getItem('gui.top-right-tile-div.style.width');
	var bottom_tile_div_style_width = sessionStorage.getItem('gui.bottom-tile-div.style.width');
	var top_div_style_height = sessionStorage.getItem('gui.top-div.style.height');
	var bottom_tile_div_style_height = sessionStorage.getItem('gui.bottom-tile-div.style.height');
	
	if(left_tile_div_style_width !== null) this.left_tile_div.style.width = left_tile_div_style_width;
	if(right_div_style_width !== null) this.right_div.style.width = right_div_style_width;
	if(top_middle_tile_div_style_width !== null) this.top_middle_tile_div.style.width = top_middle_tile_div_style_width;
	if(top_right_tile_div_style_width !== null) this.top_right_tile_div.style.width = top_right_tile_div_style_width;
	if(bottom_tile_div_style_width !== null) this.bottom_tile_div.style.width = bottom_tile_div_style_width;
	if(top_div_style_height !== null) this.top_div.style.height = top_div_style_height;
	if(bottom_tile_div_style_height !== null) this.bottom_tile_div.style.height = bottom_tile_div_style_height;
}

GUI.prototype.resize_content = function(content_width, content_height)
{
	var left_horiz_resizer_div_width = CompatLayer.get_element_width(this.left_horiz_resizer_div);
	var right_horiz_resizer_div_width = CompatLayer.get_element_width(this.right_horiz_resizer_div);
	var left_tab_headers_div_height = CompatLayer.get_element_height(this.left_tab_headers_div);
	var top_div_height = CompatLayer.get_element_height(this.top_div);
	var top_middle_tab_headers_div_height = CompatLayer.get_element_height(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = CompatLayer.get_element_height(this.top_right_tab_headers_div);
	var vert_resizer_div_height = CompatLayer.get_element_height(this.vert_resizer_div);
	var bottom_tab_headers_div_height = CompatLayer.get_element_height(this.bottom_tab_headers_div);
	
	var left_tile_div_width = CompatLayer.get_element_width(this.left_tile_div);
	var right_div_width = CompatLayer.get_element_width(this.right_div);
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	var top_right_tile_div_width = CompatLayer.get_element_width(this.top_right_tile_div);
	
	var min_content_width = this.min_tab_content_width + left_horiz_resizer_div_width + this.min_tab_content_width + right_horiz_resizer_div_width + this.min_tab_content_width;
	var min_content_height = Math.max(left_tab_headers_div_height + this.min_tab_content_height, Math.max(top_middle_tab_headers_div_height, top_right_tab_headers_div_height) + this.min_tab_content_height + vert_resizer_div_height + bottom_tab_headers_div_height + this.min_tab_content_height);
	
	if(content_width < min_content_width)
	{
		content_width = min_content_width;
	}
	
	if(content_height < min_content_height)
	{
		content_height = min_content_height;
	}
	
	top_right_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_tile_div_width - left_horiz_resizer_div_width - top_middle_tile_div_width - right_horiz_resizer_div_width);
	top_middle_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_tile_div_width - left_horiz_resizer_div_width - right_horiz_resizer_div_width - top_right_tile_div_width);
	bottom_tile_div_height = Math.max(this.min_tab_content_height + bottom_tab_headers_div_height, content_height - vert_resizer_div_height - top_div_height);
	top_div_height = Math.max(this.min_tab_content_height + Math.max(top_middle_tab_headers_div_height, top_right_tab_headers_div_height), content_height - vert_resizer_div_height - bottom_tile_div_height);
	
	var right_div_width = top_middle_tile_div_width + right_horiz_resizer_div_width + top_right_tile_div_width;
	var left_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_horiz_resizer_div_width - right_div_width);

	var left_tab_contents_div_height = Math.max(this.min_tab_content_height, content_height - left_tab_headers_div_height);
	var top_middle_tab_contents_div_height = Math.max(this.min_tab_content_height, top_div_height - top_middle_tab_headers_div_height);
	var top_right_tab_contents_div_height = Math.max(this.min_tab_content_height, top_div_height - top_right_tab_headers_div_height);
	var bottom_tab_contents_div_height = Math.max(this.min_tab_content_height, bottom_tile_div_height - bottom_tab_headers_div_height);
	
	this.content_div.style.width = content_width + 'px';
	this.content_div.style.height = content_height + 'px';
	this.left_tile_div.style.width = left_tile_div_width + 'px';
	this.left_tile_div.style.height = content_height + 'px';
	this.left_tab_contents_div.style.width = left_tile_div_width + 'px';
	this.left_tab_contents_div.style.height = left_tab_contents_div_height + 'px';
	this.left_horiz_resizer_div.style.height = content_height + 'px';
	this.right_div.style.width = right_div_width + 'px';
	this.right_div.style.height = content_height + 'px';
	this.top_div.style.width = right_div_width + 'px';
	this.top_div.style.height = top_div_height + 'px';
	this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
	this.top_middle_tile_div.style.height = top_div_height + 'px';
	this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
	this.top_middle_tab_contents_div.style.height = top_middle_tab_contents_div_height + 'px';
	this.right_horiz_resizer_div.style.height = top_div_height + 'px';
	this.top_right_tile_div.style.width = top_right_tile_div_width + 'px';
	this.top_right_tile_div.style.height = top_div_height + 'px';
	this.top_right_tab_contents_div.style.width = top_right_tile_div_width + 'px';
	this.top_right_tab_contents_div.style.height = top_right_tab_contents_div_height + 'px';
	this.vert_resizer_div.style.width = right_div_width + 'px';
	this.bottom_tile_div.style.width = right_div_width + 'px';
	this.bottom_tile_div.style.height = bottom_tile_div_height + 'px';
	this.bottom_tab_contents_div.style.width = right_div_width + 'px';
	this.bottom_tab_contents_div.style.height = bottom_tab_contents_div_height + 'px';
	
	this.save_layout();
}

GUI.prototype.resize = function()
{
	this.overlay.style.width = this.window_inner_width + 'px';
	this.overlay.style.height = this.window_inner_height + 'px';
	var toolbar_div_height = CompatLayer.get_element_height(this.toolbar_div);
	this.toolbar_div.style.width = this.window_inner_width + 'px';
	var statusbar_div_height = CompatLayer.get_element_height(this.statusbar_div);
	this.statusbar_div.style.width = this.window_inner_width + 'px';
	this.statusbar_div.style.top = Math.max(this.window_inner_height - statusbar_div_height, toolbar_div_height) + 'px';
	this.resize_content(this.window_inner_width, Math.max(this.window_inner_height - toolbar_div_height - statusbar_div_height, toolbar_div_height + statusbar_div_height));
}

GUI.prototype.monitor_window_size = function()
{
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != this.window_inner_width) || (new_window_inner_height != this.window_inner_height))
	{
		this.window_inner_width = new_window_inner_width;
		this.window_inner_height = new_window_inner_height;
		this.resize();
	}
}

GUI.prototype.gen_unique_tab_name = function(seed)
{
	var name = seed;
	var i = 0;
	while(this.find_tab_by_name(name) || this.find_tab_config_history_by_name(name))
	{
		name = seed + '_' + i;
		i++;
	}
	
	return name;
}

GUI.prototype.find_tab_by_uri = function(uri_str)
{
	var uri = new URI(uri_str);
// 	console.log('open_tab: \'' + uri_str + '\' => \'' + uri.toString());
	return this.left_tile.find_tab_by_uri(uri) || this.top_middle_tile.find_tab_by_uri(uri) || this.top_right_tile.find_tab_by_uri(uri) || this.bottom_tile.find_tab_by_uri(uri);
}

GUI.prototype.find_tab_by_name = function(name)
{
	return this.left_tile.find_tab_by_name(name) || this.top_middle_tile.find_tab_by_name(name) || this.top_right_tile.find_tab_by_name(name) || this.bottom_tile.find_tab_by_name(name);
}

GUI.prototype.find_tab_config_history_by_name = function(name)
{
	return this.left_tile.find_tab_config_history_by_name(name) || this.top_middle_tile.find_tab_config_history_by_name(name) || this.top_right_tile.find_tab_config_history_by_name(name) || this.bottom_tile.find_tab_config_history_by_name(name);
}

GUI.prototype.find_tab_by_iframe_name = function(iframe_name)
{
	return this.left_tile.find_tab_by_iframe_name(iframe_name) || this.top_middle_tile.find_tab_by_iframe_name(iframe_name) || this.top_right_tile.find_tab_by_iframe_name(iframe_name) || this.bottom_tile.find_tab_by_iframe_name(iframe_name);
}

GUI.prototype.create_context_menu = function(x, y, context_menu_items, action_this, on_destroy_action)
{
	this.destroy_context_menu();
	this.context_menu = new ContextMenu(x, y, context_menu_items, action_this, on_destroy_action);
	this.enable_events(false);
}

GUI.prototype.destroy_context_menu = function()
{
	if(this.context_menu)
	{
		this.context_menu.destroy();
		this.context_menu = null;
		this.enable_events(true);
	}
}

GUI.prototype.move_tab_to_tile = function(tab, tile_name)
{
	var destination = null;
	switch(tile_name)
	{
		case 'left-tile':
			destination = this.left_tile;
			break;
		case 'top-middle-tile':
			destination = this.top_middle_tile;
			break;
		case 'top-right-tile':
			destination = this.top_right_tile;
			break;
		case 'bottom-tile':
			destination = this.bottom_tile;
			break;
	}
	
	if(destination)
	{
		tab.detach();
		tab.attach(destination);
		tab.switch_to();
	}
}

GUI.prototype.move_tab_to_new_window = function(tab)
{
	var wnd = window.open(tab.tab_config.uri.toString());
	if(wnd)
	{
		tab.close();
		this.child_windows.push(new ChildWindow(wnd));
	}
}

GUI.prototype.for_each_tab = function(callback, callback_this)
{
	this.left_tile.for_each_tab(callback, callback_this);
	this.top_middle_tile.for_each_tab(callback, callback_this);
	this.top_right_tile.for_each_tab(callback, callback_this);
	this.bottom_tile.for_each_tab(callback, callback_this);
}

GUI.prototype.for_each_child_window = function(callback)
{
	this.child_windows.forEach(function(child_window) { if(!child_window.closed()) callback(child_window); } );
}

GUI.prototype.monitor_child_windows = function()
{
	var i = 0;
	while(i < this.child_windows.length)
	{
		var child_window = this.child_windows[i];
		
		if(child_window.closed())
		{
			this.child_windows.splice(i, 1);
		}
		else
		{
			i++;
		}
	}
}

GUI.prototype.find_statusbar_item_by_name = function(name)
{
	return this.statusbar_items.find(function(other_item) { return other_item.name == name; });
}

GUI.prototype.auto_reload_tab = function(tab)
{
	this.refresh_scheduler.auto_reload_tab(tab);
}

FileUploader.prototype.form = null;
FileUploader.prototype.input_file = null;

function FileUploader(action, accept, multiple)
{
	this.form = document.createElement('form');
	this.form.setAttribute('action', action);
	this.form.setAttribute('method', 'post');
	this.form.setAttribute('enctype', 'multipart/form-data');
	this.form.setAttribute('style', 'display:none');
	
	this.input_file = document.createElement('input');
	this.input_file.setAttribute('type', 'file');
	this.input_file.setAttribute('name', 'file');
	this.input_file.setAttribute('accept', accept);
	this.input_file.setAttribute('multiple', multiple ? 'true' : 'false');
	
	this.form.appendChild(this.input_file);
	document.body.appendChild(this.form);
	
	this.input_file.addEventListener('change', this.on_change.bind(this));
	
	this.input_file.click();
}

FileUploader.prototype.destroy = function()
{
	document.body.removeChild(this.form);
}

FileUploader.prototype.on_change = function()
{
	this.form.submit();
	this.destroy();
}

var gui = null;
document.addEventListener('DOMContentLoaded', function(event) { gui = new GUI(); });
