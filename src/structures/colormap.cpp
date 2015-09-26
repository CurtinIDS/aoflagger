#include "colormap.h"

ColorMap::ColorMap()
{
}

ColorMap::~ColorMap()
{
}

ColorMap *ColorMap::CreateColorMap(const std::string &type) throw()
{
	if(type == "coldhot")
		return new ColdHotMap();
	else if(type == "redblue")
		return new RedBlueMap();
	else if(type == "redwhiteblue")
		return new RedWhiteBlueMap();
	else if(type=="redyellowblue")
		return new RedYellowBlueMap();
	else if(type=="fire")
		return new FireMap();
	else if(type == "positive")
		return new PositiveMap();
	else if(type == "invpositive")
		return new InvPositiveMap();
	else if(type == "contrast")
		return new ContrastMap("monochrome");
	else if(type == "redbluecontrast")
		return new ContrastMap("redblue");
	else if(type == "redyellowbluecontrast")
		return new ContrastMap("redyellowblue");
	else if(type == "coldhotcontrast")
		return new ContrastMap("coldhot");
	else if(type == "positivecontrast")
		return new ContrastMap("positive");
	else if(type == "invpositivecontrast")
		return new ContrastMap("invpositive");
	else
		return new MonochromeMap();
}

const std::string ColorMap::_colorMapsString("monochrome, coldhot, redblue, redyellowblue, fire, contrast, coldhotcontrast, redbluecontrast, redyellowbluecontrast, positive, invpositive, positivecontrast, invpositivecontrast");

const std::string &ColorMap::GetColorMapsString() throw()
{
	return _colorMapsString;
}
