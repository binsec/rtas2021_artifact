URI.prototype.scheme = null;
URI.prototype.authority = null;
URI.prototype.path = null;
URI.prototype.query = null;
URI.prototype.fragment = null;

function URI(uri, window_context)
{
	if(uri instanceof URI)
	{
		this.scheme = uri.scheme;
		this.authority = uri.authority;
		this.path = uri.path;
		this.query = uri.query;
		this.fragment = uri.fragment;
	}
	else if(typeof uri === 'string')
	{
		if(uri)
		{
			var trimed_uri = uri.trim();
			var fragment_match = trimed_uri.match(new RegExp('#.*$'));
			if(fragment_match)
			{
				var longest_fragment_match = fragment_match[0];
				for(var i = 1; i < fragment_match.length; i++)
				{
					if(fragment_match[i].length > longest_fragment_match.length)
					{
						longest_fragment_match = fragment_match[i];
					}
				}
				this.fragment = longest_fragment_match.substr(1).trim();
				trimed_uri = trimed_uri.substr(0, trimed_uri.length - longest_fragment_match.length).trim();
			}
			else
			{
				this.fragment = null;
			}
			
			var query_match = trimed_uri.match(new RegExp('\\?.*$'));
			if(query_match)
			{
				var longest_query_match = query_match[0];
				for(var i = 1; i < query_match.length; i++)
				{
					if(query_match[i].length > longest_query_match.length)
					{
						longest_query_match = query_match[i];
					}
				}
				this.query = longest_query_match.substr(1).trim();
				trimed_uri = trimed_uri.substr(0, trimed_uri.length - longest_query_match.length).trim();
			}
			else
			{
				this.query = null;
			}
			
			var scheme_match = trimed_uri.match(new RegExp('^[a-z][a-z0-9]*:\/\/'));
			if(scheme_match)
			{
				var shortest_scheme_match = scheme_match[0];
				for(var i = 1; i < scheme_match.length; i++)
				{
					if(scheme_match[i].length < shortest_scheme_match.length)
					{
						shortest_scheme_match = scheme_match[i];
					}
				}
				this.scheme = shortest_scheme_match.substr(0, shortest_scheme_match.length - 3).trim();
				trimed_uri = trimed_uri.substr(shortest_scheme_match.length).trim();
			}
			else
			{
				this.scheme = null;
			}
			
			var delimiter_pos = trimed_uri.indexOf('/');
			if(this.scheme)
			{
				// absolute URI
				if(delimiter_pos != -1)
				{
					this.authority = trimed_uri.substr(0, delimiter_pos).trim();
					trimed_uri = trimed_uri.substr(delimiter_pos + 1).trim();
				}
				else
				{
					this.authority = trimed_uri;
					trimed_uri = '';
				}
				this.path = '';
			}
			else if(delimiter_pos == 0)
			{
				// URI relative to server root
				var wnd_ctx = window_context || window;
				this.authority = wnd_ctx.location.host;
				this.scheme = wnd_ctx.location.protocol.slice(0, -1);
				this.path = '';
			}
			else
			{
				// URI relative to current directory
				var wnd_ctx = window_context || window;
				this.authority = wnd_ctx.location.host;
				this.scheme = wnd_ctx.location.protocol.slice(0, -1);
				this.path = '';
				trimed_uri = wnd_ctx.location.pathname.substr(0, wnd_ctx.location.pathname.lastIndexOf('/')).trim() + '/' + trimed_uri;
			}
			
			var stack_underflow = false;
			var stack = new Array();
			var parts = trimed_uri.split('/');
			for(var i = 0; i < parts.length; i++)
			{
				if(parts[i] != '.')
				{
					if(parts[i] == '..')
					{
						if(stack.length != 0)
						{
							stack.pop();
						}
						else
						{
							stack_underflow = true;
							break;
						}
					}
					else
					{
						var part = parts[i].trim();
						if(part.length != 0)
						{
							stack.push(part);
						}
					}
				}
			}
			if(!stack_underflow)
			{
				this.path += '/' + stack.join('/');
			}
		}
		else
		{
			this.scheme = null;
			this.authority = null;
			this.path = null;
			this.query = null;
			this.fragment = null;
		}
	}
}

URI.prototype.toString = function()
{
	return (this.scheme ? (this.scheme + '://') : '') + (this.authority ? this.authority : '') + (this.path ? this.path : '') + (this.query ? ('?' + this.query) : '') + (this.fragment ? ('#' + this.fragment) : '');
}

URI.prototype.compare = function(uri)
{
	return (this.scheme == uri.scheme) && (this.authority == uri.authority) && (this.path == uri.path) && (this.query == uri.query) && (this.fragment == uri.fragment);
}
