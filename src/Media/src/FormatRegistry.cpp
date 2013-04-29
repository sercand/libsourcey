//
// LibSourcey
// Copyright (C) 2005, Sourcey <http://sourcey.com>
//
// LibSourcey is is distributed under a dual license that allows free, 
// open source use and closed source use under a standard commercial
// license.
//
// Non-Commercial Use:
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Commercial Use:
// Please contact mail@sourcey.com
//


#include "Sourcey/Media/FormatRegistry.h"

#include "Poco/SingletonHolder.h"
#include "Poco/Format.h"


using namespace std;
using Poco::SingletonHolder;
using Poco::FastMutex;
using Poco::format;


namespace Sourcey {
namespace Media {


FormatRegistry::FormatRegistry()
{
	cout << "[FormatRegistry:" << this << "] Creating" << endl;
}


FormatRegistry::~FormatRegistry()
{
	cout << "[FormatRegistry:" << this << "] Destroying" << endl;
}


FormatRegistry& FormatRegistry::instance() 
{
	static SingletonHolder<FormatRegistry> sh;
	return *sh.get();
}


Format& FormatRegistry::get(const string& name) 
{
	FastMutex::ScopedLock lock(_mutex);
	for (unsigned int i = 0; i < _formats.size(); i++) {
		if (_formats[i].name == name) {
			return _formats[i];
		}
	}
           
	throw Poco::NotFoundException("No media format for: " + name);
}


Format& FormatRegistry::getByID(const string& id) 
{
	FastMutex::ScopedLock lock(_mutex);
	for (unsigned int i = 0; i < _formats.size(); i++) {
		if (_formats[i].id == id) {
			return _formats[i];
		}
	}
           
	throw Poco::NotFoundException("No media format type: " + id);
}


Format& FormatRegistry::getOrDefault(const string& name) 
{
	{
		FastMutex::ScopedLock lock(_mutex);
		for (unsigned int i = 0; i < _formats.size(); i++) {
			if (_formats[i].name == name) {
				return _formats[i];
			}
		}
	}
           
	return getDefault();
}


Format& FormatRegistry::getDefault() 
{
	FastMutex::ScopedLock lock(_mutex);
	if (!_default.empty()) {
		return get(_default);
	}
	else if (!_formats.empty()) {
		return *_formats.begin();
	}
         
	throw Poco::NotFoundException("No default media format.");
}


bool FormatRegistry::exists(const string& name)
{
	FastMutex::ScopedLock lock(_mutex);
	for (unsigned int i = 0; i < _formats.size(); i++) {
		if (_formats[i].name == name) {
			return true;
		}
	}
           
	return false;
}


void FormatRegistry::clear()
{
	FastMutex::ScopedLock lock(_mutex);
	_formats.clear();
}


FormatList FormatRegistry::formats() const
{ 
	FastMutex::ScopedLock lock(_mutex);
	return _formats; 
}


void FormatRegistry::registerFormat(const Format& format)	
{ 
	unregisterFormat(format.name);
	FastMutex::ScopedLock lock(_mutex);
    _formats.push_back(format);
}


bool FormatRegistry::unregisterFormat(const string& name)	
{ 
	FastMutex::ScopedLock lock(_mutex);
	for (FormatList::iterator it = _formats.begin(); it != _formats.end(); ++it) {
		if ((*it).name == name) {
			_formats.erase(it);
			if (_default == name)
				_default = "";
			return true;
		}
	}
	return false;
}


void FormatRegistry::setDefault(const string& name)
{
	FastMutex::ScopedLock lock(_mutex);
	_default = name;
}


} } // namespace Sourcey::Media
