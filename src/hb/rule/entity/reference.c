#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

// Sourced from https://dev.w3.org/html5/html-author/charref at
// 2018-07-02T10:00:00Z

// HTML entities are case sensitive.
static hb_map_entity_references* hb_rule_entity_reference_map;

void hb_rule_entity_reference_map_add_entries(hb_map_entity_references* map)
{
	hb_map_entity_references_set_whole_array(map, nh_litarr("AElig"), 0xc6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("AMP"), 0x26);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Aacute"),
						 0xc1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Abreve"),
						 0x102);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Acirc"), 0xc2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Acy"), 0x410);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Afr"),
						 0x1d504);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Agrave"),
						 0xc0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Alpha"),
						 0x391);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Amacr"),
						 0x100);
	hb_map_entity_references_set_whole_array(map, nh_litarr("And"), 0x2a53);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Aogon"),
						 0x104);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Aopf"),
						 0x1d538);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ApplyFunction"), 0x2061);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Aring"), 0xc5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ascr"),
						 0x1d49c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Assign"),
						 0x2254);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Atilde"),
						 0xc3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Auml"), 0xc4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Backslash"),
						 0x2216);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Barv"),
						 0x2ae7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Barwed"),
						 0x2306);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bcy"), 0x411);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Because"),
						 0x2235);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bernoullis"),
						 0x212c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Beta"), 0x392);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bfr"),
						 0x1d505);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bopf"),
						 0x1d539);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Breve"),
						 0x2d8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bscr"),
						 0x212c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Bumpeq"),
						 0x224e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CHcy"), 0x427);
	hb_map_entity_references_set_whole_array(map, nh_litarr("COPY"), 0xa9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cacute"),
						 0x106);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cap"), 0x22d2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("CapitalDifferentialD"), 0x2145);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cayleys"),
						 0x212d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ccaron"),
						 0x10c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ccedil"),
						 0xc7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ccirc"),
						 0x108);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cconint"),
						 0x2230);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cdot"), 0x10a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cedilla"),
						 0xb8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CenterDot"),
						 0xb7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cfr"), 0x212d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Chi"), 0x3a7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CircleDot"),
						 0x2299);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CircleMinus"),
						 0x2296);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CirclePlus"),
						 0x2295);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CircleTimes"),
						 0x2297);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ClockwiseContourIntegral"), 0x2232);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("CloseCurlyDoubleQuote"), 0x201d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("CloseCurlyQuote"), 0x2019);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Colon"),
						 0x2237);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Colone"),
						 0x2a74);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Congruent"),
						 0x2261);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Conint"),
						 0x222f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ContourIntegral"), 0x222e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Copf"),
						 0x2102);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Coproduct"),
						 0x2210);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("CounterClockwiseContourIntegral"), 0x2233);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cross"),
						 0x2a2f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cscr"),
						 0x1d49e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Cup"), 0x22d3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("CupCap"),
						 0x224d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DD"), 0x2145);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DDotrahd"),
						 0x2911);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DJcy"), 0x402);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DScy"), 0x405);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DZcy"), 0x40f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dagger"),
						 0x2021);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Darr"),
						 0x21a1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dashv"),
						 0x2ae4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dcaron"),
						 0x10e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dcy"), 0x414);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Del"), 0x2207);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Delta"),
						 0x394);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dfr"),
						 0x1d507);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DiacriticalAcute"), 0xb4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DiacriticalDot"), 0x2d9);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DiacriticalDoubleAcute"), 0x2dd);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DiacriticalGrave"), 0x60);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DiacriticalTilde"), 0x2dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Diamond"),
						 0x22c4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DifferentialD"), 0x2146);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dopf"),
						 0x1d53b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dot"), 0xa8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DotDot"),
						 0x20dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DotEqual"),
						 0x2250);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleContourIntegral"), 0x222f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DoubleDot"),
						 0xa8);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleDownArrow"), 0x21d3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLeftArrow"), 0x21d0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLeftRightArrow"), 0x21d4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLeftTee"), 0x2ae4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLongLeftArrow"), 0x27f8);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLongLeftRightArrow"), 0x27fa);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleLongRightArrow"), 0x27f9);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleRightArrow"), 0x21d2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleRightTee"), 0x22a8);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleUpArrow"), 0x21d1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleUpDownArrow"), 0x21d5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DoubleVerticalBar"), 0x2225);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DownArrow"),
						 0x2193);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DownArrowBar"),
						 0x2913);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownArrowUpArrow"), 0x21f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DownBreve"),
						 0x311);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownLeftRightVector"), 0x2950);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownLeftTeeVector"), 0x295e);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownLeftVector"), 0x21bd);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownLeftVectorBar"), 0x2956);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownRightTeeVector"), 0x295f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownRightVector"), 0x21c1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("DownRightVectorBar"), 0x2957);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DownTee"),
						 0x22a4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("DownTeeArrow"),
						 0x21a7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Downarrow"),
						 0x21d3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dscr"),
						 0x1d49f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Dstrok"),
						 0x110);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ENG"), 0x14a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ETH"), 0xd0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Eacute"),
						 0xc9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ecaron"),
						 0x11a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ecirc"), 0xca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ecy"), 0x42d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Edot"), 0x116);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Efr"),
						 0x1d508);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Egrave"),
						 0xc8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Element"),
						 0x2208);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Emacr"),
						 0x112);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("EmptySmallSquare"), 0x25fb);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("EmptyVerySmallSquare"), 0x25ab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Eogon"),
						 0x118);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Eopf"),
						 0x1d53c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Epsilon"),
						 0x395);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Equal"),
						 0x2a75);
	hb_map_entity_references_set_whole_array(map, nh_litarr("EqualTilde"),
						 0x2242);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Equilibrium"),
						 0x21cc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Escr"),
						 0x2130);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Esim"),
						 0x2a73);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Eta"), 0x397);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Euml"), 0xcb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Exists"),
						 0x2203);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ExponentialE"),
						 0x2147);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Fcy"), 0x424);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ffr"),
						 0x1d509);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("FilledSmallSquare"), 0x25fc);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("FilledVerySmallSquare"), 0x25aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Fopf"),
						 0x1d53d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ForAll"),
						 0x2200);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Fouriertrf"),
						 0x2131);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Fscr"),
						 0x2131);
	hb_map_entity_references_set_whole_array(map, nh_litarr("GJcy"), 0x403);
	hb_map_entity_references_set_whole_array(map, nh_litarr("GT"), 0x3e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gamma"),
						 0x393);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gammad"),
						 0x3dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gbreve"),
						 0x11e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gcedil"),
						 0x122);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gcirc"),
						 0x11c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gcy"), 0x413);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gdot"), 0x120);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gfr"),
						 0x1d50a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gg"), 0x22d9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gopf"),
						 0x1d53e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("GreaterEqual"),
						 0x2265);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("GreaterEqualLess"), 0x22db);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("GreaterFullEqual"), 0x2267);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("GreaterGreater"), 0x2aa2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("GreaterLess"),
						 0x2277);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("GreaterSlantEqual"), 0x2a7e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("GreaterTilde"),
						 0x2273);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gscr"),
						 0x1d4a2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Gt"), 0x226b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("HARDcy"),
						 0x42a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hacek"),
						 0x2c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hat"), 0x5e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hcirc"),
						 0x124);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hfr"), 0x210c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("HilbertSpace"),
						 0x210b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hopf"),
						 0x210d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("HorizontalLine"), 0x2500);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hscr"),
						 0x210b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Hstrok"),
						 0x126);
	hb_map_entity_references_set_whole_array(map, nh_litarr("HumpDownHump"),
						 0x224e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("HumpEqual"),
						 0x224f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("IEcy"), 0x415);
	hb_map_entity_references_set_whole_array(map, nh_litarr("IJlig"),
						 0x132);
	hb_map_entity_references_set_whole_array(map, nh_litarr("IOcy"), 0x401);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iacute"),
						 0xcd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Icirc"), 0xce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Icy"), 0x418);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Idot"), 0x130);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ifr"), 0x2111);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Igrave"),
						 0xcc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Im"), 0x2111);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Imacr"),
						 0x12a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ImaginaryI"),
						 0x2148);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Implies"),
						 0x21d2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Int"), 0x222c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Integral"),
						 0x222b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Intersection"),
						 0x22c2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("InvisibleComma"), 0x2063);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("InvisibleTimes"), 0x2062);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iogon"),
						 0x12e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iopf"),
						 0x1d540);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iota"), 0x399);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iscr"),
						 0x2110);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Itilde"),
						 0x128);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iukcy"),
						 0x406);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Iuml"), 0xcf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jcirc"),
						 0x134);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jcy"), 0x419);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jfr"),
						 0x1d50d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jopf"),
						 0x1d541);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jscr"),
						 0x1d4a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jsercy"),
						 0x408);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Jukcy"),
						 0x404);
	hb_map_entity_references_set_whole_array(map, nh_litarr("KHcy"), 0x425);
	hb_map_entity_references_set_whole_array(map, nh_litarr("KJcy"), 0x40c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kappa"),
						 0x39a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kcedil"),
						 0x136);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kcy"), 0x41a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kfr"),
						 0x1d50e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kopf"),
						 0x1d542);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Kscr"),
						 0x1d4a6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LJcy"), 0x409);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LT"), 0x3c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lacute"),
						 0x139);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lambda"),
						 0x39b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lang"),
						 0x27ea);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Laplacetrf"),
						 0x2112);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Larr"),
						 0x219e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lcaron"),
						 0x13d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lcedil"),
						 0x13b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lcy"), 0x41b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftAngleBracket"), 0x27e8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftArrow"),
						 0x2190);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftArrowBar"),
						 0x21e4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftArrowRightArrow"), 0x21c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftCeiling"),
						 0x2308);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftDoubleBracket"), 0x27e6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftDownTeeVector"), 0x2961);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftDownVector"), 0x21c3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftDownVectorBar"), 0x2959);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftFloor"),
						 0x230a);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftRightArrow"), 0x2194);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftRightVector"), 0x294e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftTee"),
						 0x22a3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftTeeArrow"),
						 0x21a4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftTeeVector"), 0x295a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftTriangle"),
						 0x22b2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftTriangleBar"), 0x29cf);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftTriangleEqual"), 0x22b4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftUpDownVector"), 0x2951);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftUpTeeVector"), 0x2960);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftUpVector"),
						 0x21bf);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftUpVectorBar"), 0x2958);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LeftVector"),
						 0x21bc);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LeftVectorBar"), 0x2952);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Leftarrow"),
						 0x21d0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("Leftrightarrow"), 0x21d4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LessEqualGreater"), 0x22da);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LessFullEqual"), 0x2266);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LessGreater"),
						 0x2276);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LessLess"),
						 0x2aa1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LessSlantEqual"), 0x2a7d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("LessTilde"),
						 0x2272);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lfr"),
						 0x1d50f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ll"), 0x22d8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lleftarrow"),
						 0x21da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lmidot"),
						 0x13f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LongLeftArrow"), 0x27f5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LongLeftRightArrow"), 0x27f7);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LongRightArrow"), 0x27f6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("Longleftarrow"), 0x27f8);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("Longleftrightarrow"), 0x27fa);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("Longrightarrow"), 0x27f9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lopf"),
						 0x1d543);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LowerLeftArrow"), 0x2199);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("LowerRightArrow"), 0x2198);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lscr"),
						 0x2112);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lsh"), 0x21b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lstrok"),
						 0x141);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Lt"), 0x226a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Map"), 0x2905);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mcy"), 0x41c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("MediumSpace"),
						 0x205f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mellintrf"),
						 0x2133);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mfr"),
						 0x1d510);
	hb_map_entity_references_set_whole_array(map, nh_litarr("MinusPlus"),
						 0x2213);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mopf"),
						 0x1d544);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mscr"),
						 0x2133);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Mu"), 0x39c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NJcy"), 0x40a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Nacute"),
						 0x143);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ncaron"),
						 0x147);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ncedil"),
						 0x145);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ncy"), 0x41d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NegativeMediumSpace"), 0x200b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NegativeThickSpace"), 0x200b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NegativeThinSpace"), 0x200b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NegativeVeryThinSpace"), 0x200b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NestedGreaterGreater"), 0x226b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NestedLessLess"), 0x226a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NewLine"),
						 0xa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Nfr"),
						 0x1d511);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NoBreak"),
						 0x2060);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NonBreakingSpace"), 0xa0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Nopf"),
						 0x2115);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Not"), 0x2aec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotCongruent"),
						 0x2262);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotCupCap"),
						 0x226d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotDoubleVerticalBar"), 0x2226);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotElement"),
						 0x2209);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotEqual"),
						 0x2260);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotExists"),
						 0x2204);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotGreater"),
						 0x226f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotGreaterEqual"), 0x2271);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotGreaterLess"), 0x2279);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotGreaterTilde"), 0x2275);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotLeftTriangle"), 0x22ea);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotLeftTriangleEqual"), 0x22ec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotLess"),
						 0x226e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotLessEqual"),
						 0x2270);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotLessGreater"), 0x2278);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotLessTilde"),
						 0x2274);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotPrecedes"),
						 0x2280);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotPrecedesSlantEqual"), 0x22e0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotReverseElement"), 0x220c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotRightTriangle"), 0x22eb);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotRightTriangleEqual"), 0x22ed);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotSquareSubsetEqual"), 0x22e2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotSquareSupersetEqual"), 0x22e3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotSubsetEqual"), 0x2288);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotSucceeds"),
						 0x2281);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotSucceedsSlantEqual"), 0x22e1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotSupersetEqual"), 0x2289);
	hb_map_entity_references_set_whole_array(map, nh_litarr("NotTilde"),
						 0x2241);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotTildeEqual"), 0x2244);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotTildeFullEqual"), 0x2247);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotTildeTilde"), 0x2249);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("NotVerticalBar"), 0x2224);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Nscr"),
						 0x1d4a9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ntilde"),
						 0xd1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Nu"), 0x39d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("OElig"),
						 0x152);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Oacute"),
						 0xd3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ocirc"), 0xd4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ocy"), 0x41e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Odblac"),
						 0x150);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ofr"),
						 0x1d512);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ograve"),
						 0xd2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Omacr"),
						 0x14c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Omega"),
						 0x3a9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Omicron"),
						 0x39f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Oopf"),
						 0x1d546);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("OpenCurlyDoubleQuote"), 0x201c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("OpenCurlyQuote"), 0x2018);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Or"), 0x2a54);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Oscr"),
						 0x1d4aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Oslash"),
						 0xd8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Otilde"),
						 0xd5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Otimes"),
						 0x2a37);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ouml"), 0xd6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("OverBar"),
						 0xaf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("OverBrace"),
						 0x23de);
	hb_map_entity_references_set_whole_array(map, nh_litarr("OverBracket"),
						 0x23b4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("OverParenthesis"), 0x23dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("PartialD"),
						 0x2202);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Pcy"), 0x41f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Pfr"),
						 0x1d513);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Phi"), 0x3a6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Pi"), 0x3a0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("PlusMinus"),
						 0xb1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("Poincareplane"), 0x210c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Popf"),
						 0x2119);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Pr"), 0x2abb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Precedes"),
						 0x227a);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("PrecedesEqual"), 0x2aaf);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("PrecedesSlantEqual"), 0x227c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("PrecedesTilde"), 0x227e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Prime"),
						 0x2033);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Product"),
						 0x220f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Proportion"),
						 0x2237);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Proportional"),
						 0x221d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Pscr"),
						 0x1d4ab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Psi"), 0x3a8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("QUOT"), 0x22);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Qfr"),
						 0x1d514);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Qopf"),
						 0x211a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Qscr"),
						 0x1d4ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RBarr"),
						 0x2910);
	hb_map_entity_references_set_whole_array(map, nh_litarr("REG"), 0xae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Racute"),
						 0x154);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rang"),
						 0x27eb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rarr"),
						 0x21a0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rarrtl"),
						 0x2916);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rcaron"),
						 0x158);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rcedil"),
						 0x156);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rcy"), 0x420);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Re"), 0x211c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ReverseElement"), 0x220b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ReverseEquilibrium"), 0x21cb);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ReverseUpEquilibrium"), 0x296f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rfr"), 0x211c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rho"), 0x3a1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightAngleBracket"), 0x27e9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RightArrow"),
						 0x2192);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightArrowBar"), 0x21e5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightArrowLeftArrow"), 0x21c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RightCeiling"),
						 0x2309);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightDoubleBracket"), 0x27e7);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightDownTeeVector"), 0x295d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightDownVector"), 0x21c2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightDownVectorBar"), 0x2955);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RightFloor"),
						 0x230b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RightTee"),
						 0x22a2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightTeeArrow"), 0x21a6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightTeeVector"), 0x295b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightTriangle"), 0x22b3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightTriangleBar"), 0x29d0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightTriangleEqual"), 0x22b5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightUpDownVector"), 0x294f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightUpTeeVector"), 0x295c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightUpVector"), 0x21be);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightUpVectorBar"), 0x2954);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RightVector"),
						 0x21c0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("RightVectorBar"), 0x2953);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rightarrow"),
						 0x21d2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ropf"),
						 0x211d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RoundImplies"),
						 0x2970);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rrightarrow"),
						 0x21db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rscr"),
						 0x211b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Rsh"), 0x21b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("RuleDelayed"),
						 0x29f4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SHCHcy"),
						 0x429);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SHcy"), 0x428);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SOFTcy"),
						 0x42c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sacute"),
						 0x15a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sc"), 0x2abc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Scaron"),
						 0x160);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Scedil"),
						 0x15e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Scirc"),
						 0x15c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Scy"), 0x421);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sfr"),
						 0x1d516);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ShortDownArrow"), 0x2193);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ShortLeftArrow"), 0x2190);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ShortRightArrow"), 0x2192);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ShortUpArrow"),
						 0x2191);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sigma"),
						 0x3a3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SmallCircle"),
						 0x2218);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sopf"),
						 0x1d54a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sqrt"),
						 0x221a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Square"),
						 0x25a1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SquareIntersection"), 0x2293);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SquareSubset"),
						 0x228f);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SquareSubsetEqual"), 0x2291);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SquareSuperset"), 0x2290);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SquareSupersetEqual"), 0x2292);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SquareUnion"),
						 0x2294);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sscr"),
						 0x1d4ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Star"),
						 0x22c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sub"), 0x22d0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Subset"),
						 0x22d0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SubsetEqual"),
						 0x2286);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Succeeds"),
						 0x227b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SucceedsEqual"), 0x2ab0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SucceedsSlantEqual"), 0x227d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SucceedsTilde"), 0x227f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("SuchThat"),
						 0x220b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sum"), 0x2211);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Sup"), 0x22d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Superset"),
						 0x2283);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("SupersetEqual"), 0x2287);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Supset"),
						 0x22d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("THORN"), 0xde);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TRADE"),
						 0x2122);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TSHcy"),
						 0x40b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TScy"), 0x426);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tab"), 0x9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tau"), 0x3a4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tcaron"),
						 0x164);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tcedil"),
						 0x162);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tcy"), 0x422);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tfr"),
						 0x1d517);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Therefore"),
						 0x2234);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Theta"),
						 0x398);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ThinSpace"),
						 0x2009);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tilde"),
						 0x223c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TildeEqual"),
						 0x2243);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("TildeFullEqual"), 0x2245);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TildeTilde"),
						 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Topf"),
						 0x1d54b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("TripleDot"),
						 0x20db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tscr"),
						 0x1d4af);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Tstrok"),
						 0x166);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uacute"),
						 0xda);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uarr"),
						 0x219f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uarrocir"),
						 0x2949);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ubrcy"),
						 0x40e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ubreve"),
						 0x16c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ucirc"), 0xdb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ucy"), 0x423);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Udblac"),
						 0x170);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ufr"),
						 0x1d518);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ugrave"),
						 0xd9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Umacr"),
						 0x16a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UnderBar"),
						 0x332);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UnderBrace"),
						 0x23df);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UnderBracket"),
						 0x23b5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("UnderParenthesis"), 0x23dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Union"),
						 0x22c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UnionPlus"),
						 0x228e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uogon"),
						 0x172);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uopf"),
						 0x1d54c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UpArrow"),
						 0x2191);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UpArrowBar"),
						 0x2912);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("UpArrowDownArrow"), 0x21c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UpDownArrow"),
						 0x2195);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("UpEquilibrium"), 0x296e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UpTee"),
						 0x22a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("UpTeeArrow"),
						 0x21a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uparrow"),
						 0x21d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Updownarrow"),
						 0x21d5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("UpperLeftArrow"), 0x2196);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("UpperRightArrow"), 0x2197);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Upsi"), 0x3d2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Upsilon"),
						 0x3a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uring"),
						 0x16e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uscr"),
						 0x1d4b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Utilde"),
						 0x168);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Uuml"), 0xdc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("VDash"),
						 0x22ab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vbar"),
						 0x2aeb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vcy"), 0x412);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vdash"),
						 0x22a9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vdashl"),
						 0x2ae6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vee"), 0x22c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Verbar"),
						 0x2016);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vert"),
						 0x2016);
	hb_map_entity_references_set_whole_array(map, nh_litarr("VerticalBar"),
						 0x2223);
	hb_map_entity_references_set_whole_array(map, nh_litarr("VerticalLine"),
						 0x7c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("VerticalSeparator"), 0x2758);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("VerticalTilde"), 0x2240);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("VeryThinSpace"), 0x200a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vfr"),
						 0x1d519);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vopf"),
						 0x1d54d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vscr"),
						 0x1d4b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Vvdash"),
						 0x22aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Wcirc"),
						 0x174);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Wedge"),
						 0x22c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Wfr"),
						 0x1d51a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Wopf"),
						 0x1d54e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Wscr"),
						 0x1d4b2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Xfr"),
						 0x1d51b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Xi"), 0x39e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Xopf"),
						 0x1d54f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Xscr"),
						 0x1d4b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("YAcy"), 0x42f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("YIcy"), 0x407);
	hb_map_entity_references_set_whole_array(map, nh_litarr("YUcy"), 0x42e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Yacute"),
						 0xdd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ycirc"),
						 0x176);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Ycy"), 0x42b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Yfr"),
						 0x1d51c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Yopf"),
						 0x1d550);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Yscr"),
						 0x1d4b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Yuml"), 0x178);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ZHcy"), 0x416);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zacute"),
						 0x179);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zcaron"),
						 0x17d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zcy"), 0x417);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zdot"), 0x17b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ZeroWidthSpace"), 0x200b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zeta"), 0x396);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zfr"), 0x2128);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zopf"),
						 0x2124);
	hb_map_entity_references_set_whole_array(map, nh_litarr("Zscr"),
						 0x1d4b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aacute"),
						 0xe1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("abreve"),
						 0x103);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ac"), 0x223e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("acd"), 0x223f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("acirc"), 0xe2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("acute"), 0xb4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("acy"), 0x430);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aelig"), 0xe6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("af"), 0x2061);
	hb_map_entity_references_set_whole_array(map, nh_litarr("afr"),
						 0x1d51e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("agrave"),
						 0xe0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("alefsym"),
						 0x2135);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aleph"),
						 0x2135);
	hb_map_entity_references_set_whole_array(map, nh_litarr("alpha"),
						 0x3b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("amacr"),
						 0x101);
	hb_map_entity_references_set_whole_array(map, nh_litarr("amalg"),
						 0x2a3f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("amp"), 0x26);
	hb_map_entity_references_set_whole_array(map, nh_litarr("and"), 0x2227);
	hb_map_entity_references_set_whole_array(map, nh_litarr("andand"),
						 0x2a55);
	hb_map_entity_references_set_whole_array(map, nh_litarr("andd"),
						 0x2a5c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("andslope"),
						 0x2a58);
	hb_map_entity_references_set_whole_array(map, nh_litarr("andv"),
						 0x2a5a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ang"), 0x2220);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ange"),
						 0x29a4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angle"),
						 0x2220);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsd"),
						 0x2221);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdaa"),
						 0x29a8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdab"),
						 0x29a9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdac"),
						 0x29aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdad"),
						 0x29ab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdae"),
						 0x29ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdaf"),
						 0x29ad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdag"),
						 0x29ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angmsdah"),
						 0x29af);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angrt"),
						 0x221f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angrtvb"),
						 0x22be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angrtvbd"),
						 0x299d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angsph"),
						 0x2222);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angst"),
						 0x212b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("angzarr"),
						 0x237c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aogon"),
						 0x105);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aopf"),
						 0x1d552);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ap"), 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("apE"), 0x2a70);
	hb_map_entity_references_set_whole_array(map, nh_litarr("apacir"),
						 0x2a6f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ape"), 0x224a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("apid"),
						 0x224b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("apos"), 0x27);
	hb_map_entity_references_set_whole_array(map, nh_litarr("approx"),
						 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("approxeq"),
						 0x224a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("aring"), 0xe5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ascr"),
						 0x1d4b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ast"), 0x2a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("asymp"),
						 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("asympeq"),
						 0x224d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("atilde"),
						 0xe3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("auml"), 0xe4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("awconint"),
						 0x2233);
	hb_map_entity_references_set_whole_array(map, nh_litarr("awint"),
						 0x2a11);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bNot"),
						 0x2aed);
	hb_map_entity_references_set_whole_array(map, nh_litarr("backcong"),
						 0x224c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("backepsilon"),
						 0x3f6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("backprime"),
						 0x2035);
	hb_map_entity_references_set_whole_array(map, nh_litarr("backsim"),
						 0x223d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("backsimeq"),
						 0x22cd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("barvee"),
						 0x22bd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("barwed"),
						 0x2305);
	hb_map_entity_references_set_whole_array(map, nh_litarr("barwedge"),
						 0x2305);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bbrk"),
						 0x23b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bbrktbrk"),
						 0x23b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bcong"),
						 0x224c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bcy"), 0x431);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bdquo"),
						 0x201e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("becaus"),
						 0x2235);
	hb_map_entity_references_set_whole_array(map, nh_litarr("because"),
						 0x2235);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bemptyv"),
						 0x29b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bepsi"),
						 0x3f6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bernou"),
						 0x212c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("beta"), 0x3b2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("beth"),
						 0x2136);
	hb_map_entity_references_set_whole_array(map, nh_litarr("between"),
						 0x226c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bfr"),
						 0x1d51f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigcap"),
						 0x22c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigcirc"),
						 0x25ef);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigcup"),
						 0x22c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigodot"),
						 0x2a00);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigoplus"),
						 0x2a01);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigotimes"),
						 0x2a02);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigsqcup"),
						 0x2a06);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigstar"),
						 0x2605);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("bigtriangledown"), 0x25bd);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("bigtriangleup"), 0x25b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("biguplus"),
						 0x2a04);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigvee"),
						 0x22c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bigwedge"),
						 0x22c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bkarow"),
						 0x290d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blacklozenge"),
						 0x29eb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blacksquare"),
						 0x25aa);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("blacktriangle"), 0x25b4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("blacktriangledown"), 0x25be);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("blacktriangleleft"), 0x25c2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("blacktriangleright"), 0x25b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blank"),
						 0x2423);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blk12"),
						 0x2592);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blk14"),
						 0x2591);
	hb_map_entity_references_set_whole_array(map, nh_litarr("blk34"),
						 0x2593);
	hb_map_entity_references_set_whole_array(map, nh_litarr("block"),
						 0x2588);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bnot"),
						 0x2310);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bopf"),
						 0x1d553);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bot"), 0x22a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bottom"),
						 0x22a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bowtie"),
						 0x22c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxDL"),
						 0x2557);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxDR"),
						 0x2554);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxDl"),
						 0x2556);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxDr"),
						 0x2553);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxH"),
						 0x2550);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxHD"),
						 0x2566);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxHU"),
						 0x2569);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxHd"),
						 0x2564);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxHu"),
						 0x2567);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxUL"),
						 0x255d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxUR"),
						 0x255a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxUl"),
						 0x255c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxUr"),
						 0x2559);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxV"),
						 0x2551);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVH"),
						 0x256c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVL"),
						 0x2563);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVR"),
						 0x2560);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVh"),
						 0x256b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVl"),
						 0x2562);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxVr"),
						 0x255f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxbox"),
						 0x29c9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxdL"),
						 0x2555);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxdR"),
						 0x2552);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxdl"),
						 0x2510);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxdr"),
						 0x250c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxh"),
						 0x2500);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxhD"),
						 0x2565);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxhU"),
						 0x2568);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxhd"),
						 0x252c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxhu"),
						 0x2534);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxminus"),
						 0x229f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxplus"),
						 0x229e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxtimes"),
						 0x22a0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxuL"),
						 0x255b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxuR"),
						 0x2558);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxul"),
						 0x2518);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxur"),
						 0x2514);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxv"),
						 0x2502);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvH"),
						 0x256a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvL"),
						 0x2561);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvR"),
						 0x255e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvh"),
						 0x253c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvl"),
						 0x2524);
	hb_map_entity_references_set_whole_array(map, nh_litarr("boxvr"),
						 0x251c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bprime"),
						 0x2035);
	hb_map_entity_references_set_whole_array(map, nh_litarr("breve"),
						 0x2d8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("brvbar"),
						 0xa6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bscr"),
						 0x1d4b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bsemi"),
						 0x204f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bsim"),
						 0x223d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bsime"),
						 0x22cd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bsol"), 0x5c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bsolb"),
						 0x29c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bull"),
						 0x2022);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bullet"),
						 0x2022);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bump"),
						 0x224e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bumpE"),
						 0x2aae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bumpe"),
						 0x224f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("bumpeq"),
						 0x224f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cacute"),
						 0x107);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cap"), 0x2229);
	hb_map_entity_references_set_whole_array(map, nh_litarr("capand"),
						 0x2a44);
	hb_map_entity_references_set_whole_array(map, nh_litarr("capbrcup"),
						 0x2a49);
	hb_map_entity_references_set_whole_array(map, nh_litarr("capcap"),
						 0x2a4b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("capcup"),
						 0x2a47);
	hb_map_entity_references_set_whole_array(map, nh_litarr("capdot"),
						 0x2a40);
	hb_map_entity_references_set_whole_array(map, nh_litarr("caret"),
						 0x2041);
	hb_map_entity_references_set_whole_array(map, nh_litarr("caron"),
						 0x2c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccaps"),
						 0x2a4d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccaron"),
						 0x10d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccedil"),
						 0xe7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccirc"),
						 0x109);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccups"),
						 0x2a4c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ccupssm"),
						 0x2a50);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cdot"), 0x10b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cedil"), 0xb8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cemptyv"),
						 0x29b2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cent"), 0xa2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("centerdot"),
						 0xb7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cfr"),
						 0x1d520);
	hb_map_entity_references_set_whole_array(map, nh_litarr("chcy"), 0x447);
	hb_map_entity_references_set_whole_array(map, nh_litarr("check"),
						 0x2713);
	hb_map_entity_references_set_whole_array(map, nh_litarr("checkmark"),
						 0x2713);
	hb_map_entity_references_set_whole_array(map, nh_litarr("chi"), 0x3c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cir"), 0x25cb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cirE"),
						 0x29c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circ"), 0x2c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circeq"),
						 0x2257);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("circlearrowleft"), 0x21ba);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("circlearrowright"), 0x21bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circledR"),
						 0xae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circledS"),
						 0x24c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circledast"),
						 0x229b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circledcirc"),
						 0x229a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("circleddash"),
						 0x229d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cire"),
						 0x2257);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cirfnint"),
						 0x2a10);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cirmid"),
						 0x2aef);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cirscir"),
						 0x29c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("clubs"),
						 0x2663);
	hb_map_entity_references_set_whole_array(map, nh_litarr("clubsuit"),
						 0x2663);
	hb_map_entity_references_set_whole_array(map, nh_litarr("colon"), 0x3a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("colone"),
						 0x2254);
	hb_map_entity_references_set_whole_array(map, nh_litarr("coloneq"),
						 0x2254);
	hb_map_entity_references_set_whole_array(map, nh_litarr("comma"), 0x2c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("commat"),
						 0x40);
	hb_map_entity_references_set_whole_array(map, nh_litarr("comp"),
						 0x2201);
	hb_map_entity_references_set_whole_array(map, nh_litarr("compfn"),
						 0x2218);
	hb_map_entity_references_set_whole_array(map, nh_litarr("complement"),
						 0x2201);
	hb_map_entity_references_set_whole_array(map, nh_litarr("complexes"),
						 0x2102);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cong"),
						 0x2245);
	hb_map_entity_references_set_whole_array(map, nh_litarr("congdot"),
						 0x2a6d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("conint"),
						 0x222e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("copf"),
						 0x1d554);
	hb_map_entity_references_set_whole_array(map, nh_litarr("coprod"),
						 0x2210);
	hb_map_entity_references_set_whole_array(map, nh_litarr("copy"), 0xa9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("copysr"),
						 0x2117);
	hb_map_entity_references_set_whole_array(map, nh_litarr("crarr"),
						 0x21b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cross"),
						 0x2717);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cscr"),
						 0x1d4b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("csub"),
						 0x2acf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("csube"),
						 0x2ad1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("csup"),
						 0x2ad0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("csupe"),
						 0x2ad2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ctdot"),
						 0x22ef);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cudarrl"),
						 0x2938);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cudarrr"),
						 0x2935);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cuepr"),
						 0x22de);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cuesc"),
						 0x22df);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cularr"),
						 0x21b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cularrp"),
						 0x293d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cup"), 0x222a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cupbrcap"),
						 0x2a48);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cupcap"),
						 0x2a46);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cupcup"),
						 0x2a4a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cupdot"),
						 0x228d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cupor"),
						 0x2a45);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curarr"),
						 0x21b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curarrm"),
						 0x293c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curlyeqprec"),
						 0x22de);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curlyeqsucc"),
						 0x22df);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curlyvee"),
						 0x22ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curlywedge"),
						 0x22cf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("curren"),
						 0xa4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("curvearrowleft"), 0x21b6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("curvearrowright"), 0x21b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cuvee"),
						 0x22ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cuwed"),
						 0x22cf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cwconint"),
						 0x2232);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cwint"),
						 0x2231);
	hb_map_entity_references_set_whole_array(map, nh_litarr("cylcty"),
						 0x232d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dArr"),
						 0x21d3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dHar"),
						 0x2965);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dagger"),
						 0x2020);
	hb_map_entity_references_set_whole_array(map, nh_litarr("daleth"),
						 0x2138);
	hb_map_entity_references_set_whole_array(map, nh_litarr("darr"),
						 0x2193);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dash"),
						 0x2010);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dashv"),
						 0x22a3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dbkarow"),
						 0x290f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dblac"),
						 0x2dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dcaron"),
						 0x10f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dcy"), 0x434);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dd"), 0x2146);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ddagger"),
						 0x2021);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ddarr"),
						 0x21ca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ddotseq"),
						 0x2a77);
	hb_map_entity_references_set_whole_array(map, nh_litarr("deg"), 0xb0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("delta"),
						 0x3b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("demptyv"),
						 0x29b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dfisht"),
						 0x297f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dfr"),
						 0x1d521);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dharl"),
						 0x21c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dharr"),
						 0x21c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("diam"),
						 0x22c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("diamond"),
						 0x22c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("diamondsuit"),
						 0x2666);
	hb_map_entity_references_set_whole_array(map, nh_litarr("diams"),
						 0x2666);
	hb_map_entity_references_set_whole_array(map, nh_litarr("die"), 0xa8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("digamma"),
						 0x3dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("disin"),
						 0x22f2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("div"), 0xf7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("divide"),
						 0xf7);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("divideontimes"), 0x22c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("divonx"),
						 0x22c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("djcy"), 0x452);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dlcorn"),
						 0x231e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dlcrop"),
						 0x230d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dollar"),
						 0x24);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dopf"),
						 0x1d555);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dot"), 0x2d9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("doteq"),
						 0x2250);
	hb_map_entity_references_set_whole_array(map, nh_litarr("doteqdot"),
						 0x2251);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dotminus"),
						 0x2238);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dotplus"),
						 0x2214);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dotsquare"),
						 0x22a1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("doublebarwedge"), 0x2306);
	hb_map_entity_references_set_whole_array(map, nh_litarr("downarrow"),
						 0x2193);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("downdownarrows"), 0x21ca);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("downharpoonleft"), 0x21c3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("downharpoonright"), 0x21c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("drbkarow"),
						 0x2910);
	hb_map_entity_references_set_whole_array(map, nh_litarr("drcorn"),
						 0x231f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("drcrop"),
						 0x230c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dscr"),
						 0x1d4b9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dscy"), 0x455);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dsol"),
						 0x29f6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dstrok"),
						 0x111);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dtdot"),
						 0x22f1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dtri"),
						 0x25bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dtrif"),
						 0x25be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("duarr"),
						 0x21f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("duhar"),
						 0x296f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dwangle"),
						 0x29a6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dzcy"), 0x45f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("dzigrarr"),
						 0x27ff);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eDDot"),
						 0x2a77);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eDot"),
						 0x2251);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eacute"),
						 0xe9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("easter"),
						 0x2a6e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ecaron"),
						 0x11b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ecir"),
						 0x2256);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ecirc"), 0xea);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ecolon"),
						 0x2255);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ecy"), 0x44d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("edot"), 0x117);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ee"), 0x2147);
	hb_map_entity_references_set_whole_array(map, nh_litarr("efDot"),
						 0x2252);
	hb_map_entity_references_set_whole_array(map, nh_litarr("efr"),
						 0x1d522);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eg"), 0x2a9a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("egrave"),
						 0xe8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("egs"), 0x2a96);
	hb_map_entity_references_set_whole_array(map, nh_litarr("egsdot"),
						 0x2a98);
	hb_map_entity_references_set_whole_array(map, nh_litarr("el"), 0x2a99);
	hb_map_entity_references_set_whole_array(map, nh_litarr("elinters"),
						 0x23e7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ell"), 0x2113);
	hb_map_entity_references_set_whole_array(map, nh_litarr("els"), 0x2a95);
	hb_map_entity_references_set_whole_array(map, nh_litarr("elsdot"),
						 0x2a97);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emacr"),
						 0x113);
	hb_map_entity_references_set_whole_array(map, nh_litarr("empty"),
						 0x2205);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emptyset"),
						 0x2205);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emptyv"),
						 0x2205);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emsp"),
						 0x2003);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emsp13"),
						 0x2004);
	hb_map_entity_references_set_whole_array(map, nh_litarr("emsp14"),
						 0x2005);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eng"), 0x14b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ensp"),
						 0x2002);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eogon"),
						 0x119);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eopf"),
						 0x1d556);
	hb_map_entity_references_set_whole_array(map, nh_litarr("epar"),
						 0x22d5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eparsl"),
						 0x29e3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eplus"),
						 0x2a71);
	hb_map_entity_references_set_whole_array(map, nh_litarr("epsi"), 0x3f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("epsilon"),
						 0x3b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("epsiv"),
						 0x3b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqcirc"),
						 0x2256);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqcolon"),
						 0x2255);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqsim"),
						 0x2242);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqslantgtr"),
						 0x2a96);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqslantless"),
						 0x2a95);
	hb_map_entity_references_set_whole_array(map, nh_litarr("equals"),
						 0x3d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("equest"),
						 0x225f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("equiv"),
						 0x2261);
	hb_map_entity_references_set_whole_array(map, nh_litarr("equivDD"),
						 0x2a78);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eqvparsl"),
						 0x29e5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("erDot"),
						 0x2253);
	hb_map_entity_references_set_whole_array(map, nh_litarr("erarr"),
						 0x2971);
	hb_map_entity_references_set_whole_array(map, nh_litarr("escr"),
						 0x212f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("esdot"),
						 0x2250);
	hb_map_entity_references_set_whole_array(map, nh_litarr("esim"),
						 0x2242);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eta"), 0x3b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("eth"), 0xf0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("euml"), 0xeb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("euro"),
						 0x20ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("excl"), 0x21);
	hb_map_entity_references_set_whole_array(map, nh_litarr("exist"),
						 0x2203);
	hb_map_entity_references_set_whole_array(map, nh_litarr("expectation"),
						 0x2130);
	hb_map_entity_references_set_whole_array(map, nh_litarr("exponentiale"),
						 0x2147);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("fallingdotseq"), 0x2252);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fcy"), 0x444);
	hb_map_entity_references_set_whole_array(map, nh_litarr("female"),
						 0x2640);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ffilig"),
						 0xfb03);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fflig"),
						 0xfb00);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ffllig"),
						 0xfb04);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ffr"),
						 0x1d523);
	hb_map_entity_references_set_whole_array(map, nh_litarr("filig"),
						 0xfb01);
	hb_map_entity_references_set_whole_array(map, nh_litarr("flat"),
						 0x266d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fllig"),
						 0xfb02);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fltns"),
						 0x25b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fnof"), 0x192);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fopf"),
						 0x1d557);
	hb_map_entity_references_set_whole_array(map, nh_litarr("forall"),
						 0x2200);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fork"),
						 0x22d4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("forkv"),
						 0x2ad9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fpartint"),
						 0x2a0d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac12"),
						 0xbd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac13"),
						 0x2153);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac14"),
						 0xbc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac15"),
						 0x2155);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac16"),
						 0x2159);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac18"),
						 0x215b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac23"),
						 0x2154);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac25"),
						 0x2156);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac34"),
						 0xbe);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac35"),
						 0x2157);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac38"),
						 0x215c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac45"),
						 0x2158);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac56"),
						 0x215a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac58"),
						 0x215d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frac78"),
						 0x215e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frasl"),
						 0x2044);
	hb_map_entity_references_set_whole_array(map, nh_litarr("frown"),
						 0x2322);
	hb_map_entity_references_set_whole_array(map, nh_litarr("fscr"),
						 0x1d4bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gE"), 0x2267);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gEl"), 0x2a8c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gacute"),
						 0x1f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gamma"),
						 0x3b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gammad"),
						 0x3dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gap"), 0x2a86);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gbreve"),
						 0x11f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gcirc"),
						 0x11d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gcy"), 0x433);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gdot"), 0x121);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ge"), 0x2265);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gel"), 0x22db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("geq"), 0x2265);
	hb_map_entity_references_set_whole_array(map, nh_litarr("geqq"),
						 0x2267);
	hb_map_entity_references_set_whole_array(map, nh_litarr("geqslant"),
						 0x2a7e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ges"), 0x2a7e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gescc"),
						 0x2aa9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gesdot"),
						 0x2a80);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gesdoto"),
						 0x2a82);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gesdotol"),
						 0x2a84);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gesles"),
						 0x2a94);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gfr"),
						 0x1d524);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gg"), 0x226b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ggg"), 0x22d9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gimel"),
						 0x2137);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gjcy"), 0x453);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gl"), 0x2277);
	hb_map_entity_references_set_whole_array(map, nh_litarr("glE"), 0x2a92);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gla"), 0x2aa5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("glj"), 0x2aa4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gnE"), 0x2269);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gnap"),
						 0x2a8a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gnapprox"),
						 0x2a8a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gne"), 0x2a88);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gneq"),
						 0x2a88);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gneqq"),
						 0x2269);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gnsim"),
						 0x22e7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gopf"),
						 0x1d558);
	hb_map_entity_references_set_whole_array(map, nh_litarr("grave"), 0x60);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gscr"),
						 0x210a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gsim"),
						 0x2273);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gsime"),
						 0x2a8e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gsiml"),
						 0x2a90);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gt"), 0x3e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtcc"),
						 0x2aa7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtcir"),
						 0x2a7a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtdot"),
						 0x22d7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtlPar"),
						 0x2995);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtquest"),
						 0x2a7c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtrapprox"),
						 0x2a86);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtrarr"),
						 0x2978);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtrdot"),
						 0x22d7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtreqless"),
						 0x22db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtreqqless"),
						 0x2a8c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtrless"),
						 0x2277);
	hb_map_entity_references_set_whole_array(map, nh_litarr("gtrsim"),
						 0x2273);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hArr"),
						 0x21d4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hairsp"),
						 0x200a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("half"), 0xbd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hamilt"),
						 0x210b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hardcy"),
						 0x44a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("harr"),
						 0x2194);
	hb_map_entity_references_set_whole_array(map, nh_litarr("harrcir"),
						 0x2948);
	hb_map_entity_references_set_whole_array(map, nh_litarr("harrw"),
						 0x21ad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hbar"),
						 0x210f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hcirc"),
						 0x125);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hearts"),
						 0x2665);
	hb_map_entity_references_set_whole_array(map, nh_litarr("heartsuit"),
						 0x2665);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hellip"),
						 0x2026);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hercon"),
						 0x22b9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hfr"),
						 0x1d525);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hksearow"),
						 0x2925);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hkswarow"),
						 0x2926);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hoarr"),
						 0x21ff);
	hb_map_entity_references_set_whole_array(map, nh_litarr("homtht"),
						 0x223b);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("hookleftarrow"), 0x21a9);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("hookrightarrow"), 0x21aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hopf"),
						 0x1d559);
	hb_map_entity_references_set_whole_array(map, nh_litarr("horbar"),
						 0x2015);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hscr"),
						 0x1d4bd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hslash"),
						 0x210f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hstrok"),
						 0x127);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hybull"),
						 0x2043);
	hb_map_entity_references_set_whole_array(map, nh_litarr("hyphen"),
						 0x2010);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iacute"),
						 0xed);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ic"), 0x2063);
	hb_map_entity_references_set_whole_array(map, nh_litarr("icirc"), 0xee);
	hb_map_entity_references_set_whole_array(map, nh_litarr("icy"), 0x438);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iecy"), 0x435);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iexcl"), 0xa1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iff"), 0x21d4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ifr"),
						 0x1d526);
	hb_map_entity_references_set_whole_array(map, nh_litarr("igrave"),
						 0xec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ii"), 0x2148);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iiiint"),
						 0x2a0c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iiint"),
						 0x222d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iinfin"),
						 0x29dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iiota"),
						 0x2129);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ijlig"),
						 0x133);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imacr"),
						 0x12b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("image"),
						 0x2111);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imagline"),
						 0x2110);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imagpart"),
						 0x2111);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imath"),
						 0x131);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imof"),
						 0x22b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("imped"),
						 0x1b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("in"), 0x2208);
	hb_map_entity_references_set_whole_array(map, nh_litarr("incare"),
						 0x2105);
	hb_map_entity_references_set_whole_array(map, nh_litarr("infin"),
						 0x221e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("infintie"),
						 0x29dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("inodot"),
						 0x131);
	hb_map_entity_references_set_whole_array(map, nh_litarr("int"), 0x222b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("intcal"),
						 0x22ba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("integers"),
						 0x2124);
	hb_map_entity_references_set_whole_array(map, nh_litarr("intercal"),
						 0x22ba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("intlarhk"),
						 0x2a17);
	hb_map_entity_references_set_whole_array(map, nh_litarr("intprod"),
						 0x2a3c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iocy"), 0x451);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iogon"),
						 0x12f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iopf"),
						 0x1d55a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iota"), 0x3b9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iprod"),
						 0x2a3c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iquest"),
						 0xbf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iscr"),
						 0x1d4be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isin"),
						 0x2208);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isinE"),
						 0x22f9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isindot"),
						 0x22f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isins"),
						 0x22f4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isinsv"),
						 0x22f3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("isinv"),
						 0x2208);
	hb_map_entity_references_set_whole_array(map, nh_litarr("it"), 0x2062);
	hb_map_entity_references_set_whole_array(map, nh_litarr("itilde"),
						 0x129);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iukcy"),
						 0x456);
	hb_map_entity_references_set_whole_array(map, nh_litarr("iuml"), 0xef);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jcirc"),
						 0x135);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jcy"), 0x439);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jfr"),
						 0x1d527);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jmath"),
						 0x237);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jopf"),
						 0x1d55b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jscr"),
						 0x1d4bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jsercy"),
						 0x458);
	hb_map_entity_references_set_whole_array(map, nh_litarr("jukcy"),
						 0x454);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kappa"),
						 0x3ba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kappav"),
						 0x3f0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kcedil"),
						 0x137);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kcy"), 0x43a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kfr"),
						 0x1d528);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kgreen"),
						 0x138);
	hb_map_entity_references_set_whole_array(map, nh_litarr("khcy"), 0x445);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kjcy"), 0x45c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kopf"),
						 0x1d55c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("kscr"),
						 0x1d4c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lAarr"),
						 0x21da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lArr"),
						 0x21d0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lAtail"),
						 0x291b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lBarr"),
						 0x290e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lE"), 0x2266);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lEg"), 0x2a8b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lHar"),
						 0x2962);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lacute"),
						 0x13a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("laemptyv"),
						 0x29b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lagran"),
						 0x2112);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lambda"),
						 0x3bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lang"),
						 0x27e8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("langd"),
						 0x2991);
	hb_map_entity_references_set_whole_array(map, nh_litarr("langle"),
						 0x27e8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lap"), 0x2a85);
	hb_map_entity_references_set_whole_array(map, nh_litarr("laquo"), 0xab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larr"),
						 0x2190);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrb"),
						 0x21e4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrbfs"),
						 0x291f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrfs"),
						 0x291d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrhk"),
						 0x21a9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrlp"),
						 0x21ab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrpl"),
						 0x2939);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrsim"),
						 0x2973);
	hb_map_entity_references_set_whole_array(map, nh_litarr("larrtl"),
						 0x21a2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lat"), 0x2aab);
	hb_map_entity_references_set_whole_array(map, nh_litarr("latail"),
						 0x2919);
	hb_map_entity_references_set_whole_array(map, nh_litarr("late"),
						 0x2aad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbarr"),
						 0x290c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbbrk"),
						 0x2772);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbrace"),
						 0x7b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbrack"),
						 0x5b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbrke"),
						 0x298b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbrksld"),
						 0x298f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lbrkslu"),
						 0x298d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lcaron"),
						 0x13e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lcedil"),
						 0x13c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lceil"),
						 0x2308);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lcub"), 0x7b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lcy"), 0x43b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldca"),
						 0x2936);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldquo"),
						 0x201c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldquor"),
						 0x201e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldrdhar"),
						 0x2967);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldrushar"),
						 0x294b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ldsh"),
						 0x21b2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("le"), 0x2264);
	hb_map_entity_references_set_whole_array(map, nh_litarr("leftarrow"),
						 0x2190);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftarrowtail"), 0x21a2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftharpoondown"), 0x21bd);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftharpoonup"), 0x21bc);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftleftarrows"), 0x21c7);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftrightarrow"), 0x2194);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftrightarrows"), 0x21c6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftrightharpoons"), 0x21cb);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftrightsquigarrow"), 0x21ad);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("leftthreetimes"), 0x22cb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("leg"), 0x22da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("leq"), 0x2264);
	hb_map_entity_references_set_whole_array(map, nh_litarr("leqq"),
						 0x2266);
	hb_map_entity_references_set_whole_array(map, nh_litarr("leqslant"),
						 0x2a7d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("les"), 0x2a7d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lescc"),
						 0x2aa8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesdot"),
						 0x2a7f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesdoto"),
						 0x2a81);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesdotor"),
						 0x2a83);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesges"),
						 0x2a93);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lessapprox"),
						 0x2a85);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lessdot"),
						 0x22d6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesseqgtr"),
						 0x22da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesseqqgtr"),
						 0x2a8b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lessgtr"),
						 0x2276);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lesssim"),
						 0x2272);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lfisht"),
						 0x297c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lfloor"),
						 0x230a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lfr"),
						 0x1d529);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lg"), 0x2276);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lgE"), 0x2a91);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lhard"),
						 0x21bd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lharu"),
						 0x21bc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lharul"),
						 0x296a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lhblk"),
						 0x2584);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ljcy"), 0x459);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ll"), 0x226a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("llarr"),
						 0x21c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("llcorner"),
						 0x231e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("llhard"),
						 0x296b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lltri"),
						 0x25fa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lmidot"),
						 0x140);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lmoust"),
						 0x23b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lmoustache"),
						 0x23b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lnE"), 0x2268);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lnap"),
						 0x2a89);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lnapprox"),
						 0x2a89);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lne"), 0x2a87);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lneq"),
						 0x2a87);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lneqq"),
						 0x2268);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lnsim"),
						 0x22e6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("loang"),
						 0x27ec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("loarr"),
						 0x21fd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lobrk"),
						 0x27e6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("longleftarrow"), 0x27f5);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("longleftrightarrow"), 0x27f7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("longmapsto"),
						 0x27fc);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("longrightarrow"), 0x27f6);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("looparrowleft"), 0x21ab);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("looparrowright"), 0x21ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lopar"),
						 0x2985);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lopf"),
						 0x1d55d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("loplus"),
						 0x2a2d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lotimes"),
						 0x2a34);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lowast"),
						 0x2217);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lowbar"),
						 0x5f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("loz"), 0x25ca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lozenge"),
						 0x25ca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lozf"),
						 0x29eb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lpar"), 0x28);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lparlt"),
						 0x2993);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrarr"),
						 0x21c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrcorner"),
						 0x231f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrhar"),
						 0x21cb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrhard"),
						 0x296d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrm"), 0x200e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lrtri"),
						 0x22bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsaquo"),
						 0x2039);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lscr"),
						 0x1d4c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsh"), 0x21b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsim"),
						 0x2272);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsime"),
						 0x2a8d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsimg"),
						 0x2a8f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsqb"), 0x5b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsquo"),
						 0x2018);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lsquor"),
						 0x201a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lstrok"),
						 0x142);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lt"), 0x3c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltcc"),
						 0x2aa6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltcir"),
						 0x2a79);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltdot"),
						 0x22d6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lthree"),
						 0x22cb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltimes"),
						 0x22c9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltlarr"),
						 0x2976);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltquest"),
						 0x2a7b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltrPar"),
						 0x2996);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltri"),
						 0x25c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltrie"),
						 0x22b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ltrif"),
						 0x25c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("lurdshar"),
						 0x294a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("luruhar"),
						 0x2966);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mDDot"),
						 0x223a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("macr"), 0xaf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("male"),
						 0x2642);
	hb_map_entity_references_set_whole_array(map, nh_litarr("malt"),
						 0x2720);
	hb_map_entity_references_set_whole_array(map, nh_litarr("maltese"),
						 0x2720);
	hb_map_entity_references_set_whole_array(map, nh_litarr("map"), 0x21a6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mapsto"),
						 0x21a6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mapstodown"),
						 0x21a7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mapstoleft"),
						 0x21a4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mapstoup"),
						 0x21a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("marker"),
						 0x25ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mcomma"),
						 0x2a29);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mcy"), 0x43c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mdash"),
						 0x2014);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("measuredangle"), 0x2221);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mfr"),
						 0x1d52a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mho"), 0x2127);
	hb_map_entity_references_set_whole_array(map, nh_litarr("micro"), 0xb5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mid"), 0x2223);
	hb_map_entity_references_set_whole_array(map, nh_litarr("midast"),
						 0x2a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("midcir"),
						 0x2af0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("middot"),
						 0xb7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("minus"),
						 0x2212);
	hb_map_entity_references_set_whole_array(map, nh_litarr("minusb"),
						 0x229f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("minusd"),
						 0x2238);
	hb_map_entity_references_set_whole_array(map, nh_litarr("minusdu"),
						 0x2a2a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mlcp"),
						 0x2adb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mldr"),
						 0x2026);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mnplus"),
						 0x2213);
	hb_map_entity_references_set_whole_array(map, nh_litarr("models"),
						 0x22a7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mopf"),
						 0x1d55e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mp"), 0x2213);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mscr"),
						 0x1d4c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mstpos"),
						 0x223e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mu"), 0x3bc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("multimap"),
						 0x22b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("mumap"),
						 0x22b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nLeftarrow"),
						 0x21cd);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("nLeftrightarrow"), 0x21ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nRightarrow"),
						 0x21cf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nVDash"),
						 0x22af);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nVdash"),
						 0x22ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nabla"),
						 0x2207);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nacute"),
						 0x144);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nap"), 0x2249);
	hb_map_entity_references_set_whole_array(map, nh_litarr("napos"),
						 0x149);
	hb_map_entity_references_set_whole_array(map, nh_litarr("napprox"),
						 0x2249);
	hb_map_entity_references_set_whole_array(map, nh_litarr("natur"),
						 0x266e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("natural"),
						 0x266e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("naturals"),
						 0x2115);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nbsp"), 0xa0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncap"),
						 0x2a43);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncaron"),
						 0x148);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncedil"),
						 0x146);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncong"),
						 0x2247);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncup"),
						 0x2a42);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ncy"), 0x43d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ndash"),
						 0x2013);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ne"), 0x2260);
	hb_map_entity_references_set_whole_array(map, nh_litarr("neArr"),
						 0x21d7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nearhk"),
						 0x2924);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nearr"),
						 0x2197);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nearrow"),
						 0x2197);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nequiv"),
						 0x2262);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nesear"),
						 0x2928);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nexist"),
						 0x2204);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nexists"),
						 0x2204);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nfr"),
						 0x1d52b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nge"), 0x2271);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ngeq"),
						 0x2271);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ngsim"),
						 0x2275);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ngt"), 0x226f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ngtr"),
						 0x226f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nhArr"),
						 0x21ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nharr"),
						 0x21ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nhpar"),
						 0x2af2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ni"), 0x220b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nis"), 0x22fc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nisd"),
						 0x22fa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("niv"), 0x220b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("njcy"), 0x45a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nlArr"),
						 0x21cd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nlarr"),
						 0x219a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nldr"),
						 0x2025);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nle"), 0x2270);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nleftarrow"),
						 0x219a);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("nleftrightarrow"), 0x21ae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nleq"),
						 0x2270);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nless"),
						 0x226e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nlsim"),
						 0x2274);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nlt"), 0x226e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nltri"),
						 0x22ea);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nltrie"),
						 0x22ec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nmid"),
						 0x2224);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nopf"),
						 0x1d55f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("not"), 0xac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notin"),
						 0x2209);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notinva"),
						 0x2209);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notinvb"),
						 0x22f7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notinvc"),
						 0x22f6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notni"),
						 0x220c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notniva"),
						 0x220c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notnivb"),
						 0x22fe);
	hb_map_entity_references_set_whole_array(map, nh_litarr("notnivc"),
						 0x22fd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("npar"),
						 0x2226);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nparallel"),
						 0x2226);
	hb_map_entity_references_set_whole_array(map, nh_litarr("npolint"),
						 0x2a14);
	hb_map_entity_references_set_whole_array(map, nh_litarr("npr"), 0x2280);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nprcue"),
						 0x22e0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nprec"),
						 0x2280);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nrArr"),
						 0x21cf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nrarr"),
						 0x219b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nrightarrow"),
						 0x219b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nrtri"),
						 0x22eb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nrtrie"),
						 0x22ed);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsc"), 0x2281);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsccue"),
						 0x22e1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nscr"),
						 0x1d4c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nshortmid"),
						 0x2224);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("nshortparallel"), 0x2226);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsim"),
						 0x2241);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsime"),
						 0x2244);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsimeq"),
						 0x2244);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsmid"),
						 0x2224);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nspar"),
						 0x2226);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsqsube"),
						 0x22e2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsqsupe"),
						 0x22e3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsub"),
						 0x2284);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsube"),
						 0x2288);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsubseteq"),
						 0x2288);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsucc"),
						 0x2281);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsup"),
						 0x2285);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsupe"),
						 0x2289);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nsupseteq"),
						 0x2289);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ntgl"),
						 0x2279);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ntilde"),
						 0xf1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ntlg"),
						 0x2278);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ntriangleleft"), 0x22ea);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ntrianglelefteq"), 0x22ec);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ntriangleright"), 0x22eb);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("ntrianglerighteq"), 0x22ed);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nu"), 0x3bd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("num"), 0x23);
	hb_map_entity_references_set_whole_array(map, nh_litarr("numero"),
						 0x2116);
	hb_map_entity_references_set_whole_array(map, nh_litarr("numsp"),
						 0x2007);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvDash"),
						 0x22ad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvHarr"),
						 0x2904);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvdash"),
						 0x22ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvinfin"),
						 0x29de);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvlArr"),
						 0x2902);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nvrArr"),
						 0x2903);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nwArr"),
						 0x21d6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nwarhk"),
						 0x2923);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nwarr"),
						 0x2196);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nwarrow"),
						 0x2196);
	hb_map_entity_references_set_whole_array(map, nh_litarr("nwnear"),
						 0x2927);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oS"), 0x24c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oacute"),
						 0xf3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oast"),
						 0x229b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ocir"),
						 0x229a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ocirc"), 0xf4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ocy"), 0x43e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("odash"),
						 0x229d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("odblac"),
						 0x151);
	hb_map_entity_references_set_whole_array(map, nh_litarr("odiv"),
						 0x2a38);
	hb_map_entity_references_set_whole_array(map, nh_litarr("odot"),
						 0x2299);
	hb_map_entity_references_set_whole_array(map, nh_litarr("odsold"),
						 0x29bc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oelig"),
						 0x153);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ofcir"),
						 0x29bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ofr"),
						 0x1d52c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ogon"), 0x2db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ograve"),
						 0xf2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ogt"), 0x29c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ohbar"),
						 0x29b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ohm"), 0x2126);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oint"),
						 0x222e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("olarr"),
						 0x21ba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("olcir"),
						 0x29be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("olcross"),
						 0x29bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oline"),
						 0x203e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("olt"), 0x29c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("omacr"),
						 0x14d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("omega"),
						 0x3c9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("omicron"),
						 0x3bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("omid"),
						 0x29b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ominus"),
						 0x2296);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oopf"),
						 0x1d560);
	hb_map_entity_references_set_whole_array(map, nh_litarr("opar"),
						 0x29b7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("operp"),
						 0x29b9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oplus"),
						 0x2295);
	hb_map_entity_references_set_whole_array(map, nh_litarr("or"), 0x2228);
	hb_map_entity_references_set_whole_array(map, nh_litarr("orarr"),
						 0x21bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ord"), 0x2a5d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("order"),
						 0x2134);
	hb_map_entity_references_set_whole_array(map, nh_litarr("orderof"),
						 0x2134);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ordf"), 0xaa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ordm"), 0xba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("origof"),
						 0x22b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oror"),
						 0x2a56);
	hb_map_entity_references_set_whole_array(map, nh_litarr("orslope"),
						 0x2a57);
	hb_map_entity_references_set_whole_array(map, nh_litarr("orv"), 0x2a5b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oscr"),
						 0x2134);
	hb_map_entity_references_set_whole_array(map, nh_litarr("oslash"),
						 0xf8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("osol"),
						 0x2298);
	hb_map_entity_references_set_whole_array(map, nh_litarr("otilde"),
						 0xf5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("otimes"),
						 0x2297);
	hb_map_entity_references_set_whole_array(map, nh_litarr("otimesas"),
						 0x2a36);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ouml"), 0xf6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ovbar"),
						 0x233d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("par"), 0x2225);
	hb_map_entity_references_set_whole_array(map, nh_litarr("para"), 0xb6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("parallel"),
						 0x2225);
	hb_map_entity_references_set_whole_array(map, nh_litarr("parsim"),
						 0x2af3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("parsl"),
						 0x2afd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("part"),
						 0x2202);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pcy"), 0x43f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("percnt"),
						 0x25);
	hb_map_entity_references_set_whole_array(map, nh_litarr("period"),
						 0x2e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("permil"),
						 0x2030);
	hb_map_entity_references_set_whole_array(map, nh_litarr("perp"),
						 0x22a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pertenk"),
						 0x2031);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pfr"),
						 0x1d52d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("phi"), 0x3c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("phiv"), 0x3c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("phmmat"),
						 0x2133);
	hb_map_entity_references_set_whole_array(map, nh_litarr("phone"),
						 0x260e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pi"), 0x3c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pitchfork"),
						 0x22d4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("piv"), 0x3d6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("planck"),
						 0x210f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("planckh"),
						 0x210e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plankv"),
						 0x210f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plus"), 0x2b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plusacir"),
						 0x2a23);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plusb"),
						 0x229e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pluscir"),
						 0x2a22);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plusdo"),
						 0x2214);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plusdu"),
						 0x2a25);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pluse"),
						 0x2a72);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plusmn"),
						 0xb1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plussim"),
						 0x2a26);
	hb_map_entity_references_set_whole_array(map, nh_litarr("plustwo"),
						 0x2a27);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pm"), 0xb1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pointint"),
						 0x2a15);
	hb_map_entity_references_set_whole_array(map, nh_litarr("popf"),
						 0x1d561);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pound"), 0xa3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pr"), 0x227a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prE"), 0x2ab3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prap"),
						 0x2ab7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prcue"),
						 0x227c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pre"), 0x2aaf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prec"),
						 0x227a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("precapprox"),
						 0x2ab7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("preccurlyeq"),
						 0x227c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("preceq"),
						 0x2aaf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("precnapprox"),
						 0x2ab9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("precneqq"),
						 0x2ab5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("precnsim"),
						 0x22e8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("precsim"),
						 0x227e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prime"),
						 0x2032);
	hb_map_entity_references_set_whole_array(map, nh_litarr("primes"),
						 0x2119);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prnE"),
						 0x2ab5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prnap"),
						 0x2ab9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prnsim"),
						 0x22e8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prod"),
						 0x220f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("profalar"),
						 0x232e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("profline"),
						 0x2312);
	hb_map_entity_references_set_whole_array(map, nh_litarr("profsurf"),
						 0x2313);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prop"),
						 0x221d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("propto"),
						 0x221d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prsim"),
						 0x227e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("prurel"),
						 0x22b0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("pscr"),
						 0x1d4c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("psi"), 0x3c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("puncsp"),
						 0x2008);
	hb_map_entity_references_set_whole_array(map, nh_litarr("qfr"),
						 0x1d52e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("qint"),
						 0x2a0c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("qopf"),
						 0x1d562);
	hb_map_entity_references_set_whole_array(map, nh_litarr("qprime"),
						 0x2057);
	hb_map_entity_references_set_whole_array(map, nh_litarr("qscr"),
						 0x1d4c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("quaternions"),
						 0x210d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("quatint"),
						 0x2a16);
	hb_map_entity_references_set_whole_array(map, nh_litarr("quest"), 0x3f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("questeq"),
						 0x225f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("quot"), 0x22);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rAarr"),
						 0x21db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rArr"),
						 0x21d2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rAtail"),
						 0x291c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rBarr"),
						 0x290f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rHar"),
						 0x2964);
	hb_map_entity_references_set_whole_array(map, nh_litarr("race"),
						 0x29da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("racute"),
						 0x155);
	hb_map_entity_references_set_whole_array(map, nh_litarr("radic"),
						 0x221a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("raemptyv"),
						 0x29b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rang"),
						 0x27e9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rangd"),
						 0x2992);
	hb_map_entity_references_set_whole_array(map, nh_litarr("range"),
						 0x29a5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rangle"),
						 0x27e9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("raquo"), 0xbb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarr"),
						 0x2192);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrap"),
						 0x2975);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrb"),
						 0x21e5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrbfs"),
						 0x2920);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrc"),
						 0x2933);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrfs"),
						 0x291e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrhk"),
						 0x21aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrlp"),
						 0x21ac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrpl"),
						 0x2945);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrsim"),
						 0x2974);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrtl"),
						 0x21a3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rarrw"),
						 0x219d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ratail"),
						 0x291a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ratio"),
						 0x2236);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rationals"),
						 0x211a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbarr"),
						 0x290d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbbrk"),
						 0x2773);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbrace"),
						 0x7d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbrack"),
						 0x5d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbrke"),
						 0x298c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbrksld"),
						 0x298e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rbrkslu"),
						 0x2990);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rcaron"),
						 0x159);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rcedil"),
						 0x157);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rceil"),
						 0x2309);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rcub"), 0x7d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rcy"), 0x440);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rdca"),
						 0x2937);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rdldhar"),
						 0x2969);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rdquo"),
						 0x201d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rdquor"),
						 0x201d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rdsh"),
						 0x21b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("real"),
						 0x211c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("realine"),
						 0x211b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("realpart"),
						 0x211c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("reals"),
						 0x211d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rect"),
						 0x25ad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("reg"), 0xae);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rfisht"),
						 0x297d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rfloor"),
						 0x230b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rfr"),
						 0x1d52f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rhard"),
						 0x21c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rharu"),
						 0x21c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rharul"),
						 0x296c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rho"), 0x3c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rhov"), 0x3f1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rightarrow"),
						 0x2192);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightarrowtail"), 0x21a3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightharpoondown"), 0x21c1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightharpoonup"), 0x21c0);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightleftarrows"), 0x21c4);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightleftharpoons"), 0x21cc);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightrightarrows"), 0x21c9);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightsquigarrow"), 0x219d);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("rightthreetimes"), 0x22cc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ring"), 0x2da);
	hb_map_entity_references_set_whole_array(map, nh_litarr("risingdotseq"),
						 0x2253);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rlarr"),
						 0x21c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rlhar"),
						 0x21cc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rlm"), 0x200f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rmoust"),
						 0x23b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rmoustache"),
						 0x23b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rnmid"),
						 0x2aee);
	hb_map_entity_references_set_whole_array(map, nh_litarr("roang"),
						 0x27ed);
	hb_map_entity_references_set_whole_array(map, nh_litarr("roarr"),
						 0x21fe);
	hb_map_entity_references_set_whole_array(map, nh_litarr("robrk"),
						 0x27e7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ropar"),
						 0x2986);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ropf"),
						 0x1d563);
	hb_map_entity_references_set_whole_array(map, nh_litarr("roplus"),
						 0x2a2e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rotimes"),
						 0x2a35);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rpar"), 0x29);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rpargt"),
						 0x2994);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rppolint"),
						 0x2a12);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rrarr"),
						 0x21c9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rsaquo"),
						 0x203a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rscr"),
						 0x1d4c7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rsh"), 0x21b1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rsqb"), 0x5d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rsquo"),
						 0x2019);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rsquor"),
						 0x2019);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rthree"),
						 0x22cc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rtimes"),
						 0x22ca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rtri"),
						 0x25b9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rtrie"),
						 0x22b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rtrif"),
						 0x25b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rtriltri"),
						 0x29ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ruluhar"),
						 0x2968);
	hb_map_entity_references_set_whole_array(map, nh_litarr("rx"), 0x211e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sacute"),
						 0x15b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sbquo"),
						 0x201a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sc"), 0x227b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scE"), 0x2ab4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scap"),
						 0x2ab8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scaron"),
						 0x161);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sccue"),
						 0x227d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sce"), 0x2ab0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scedil"),
						 0x15f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scirc"),
						 0x15d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scnE"),
						 0x2ab6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scnap"),
						 0x2aba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scnsim"),
						 0x22e9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scpolint"),
						 0x2a13);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scsim"),
						 0x227f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("scy"), 0x441);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sdot"),
						 0x22c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sdotb"),
						 0x22a1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sdote"),
						 0x2a66);
	hb_map_entity_references_set_whole_array(map, nh_litarr("seArr"),
						 0x21d8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("searhk"),
						 0x2925);
	hb_map_entity_references_set_whole_array(map, nh_litarr("searr"),
						 0x2198);
	hb_map_entity_references_set_whole_array(map, nh_litarr("searrow"),
						 0x2198);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sect"), 0xa7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("semi"), 0x3b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("seswar"),
						 0x2929);
	hb_map_entity_references_set_whole_array(map, nh_litarr("setminus"),
						 0x2216);
	hb_map_entity_references_set_whole_array(map, nh_litarr("setmn"),
						 0x2216);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sext"),
						 0x2736);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sfr"),
						 0x1d530);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sfrown"),
						 0x2322);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sharp"),
						 0x266f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("shchcy"),
						 0x449);
	hb_map_entity_references_set_whole_array(map, nh_litarr("shcy"), 0x448);
	hb_map_entity_references_set_whole_array(map, nh_litarr("shortmid"),
						 0x2223);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("shortparallel"), 0x2225);
	hb_map_entity_references_set_whole_array(map, nh_litarr("shy"), 0xad);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sigma"),
						 0x3c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sigmaf"),
						 0x3c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sigmav"),
						 0x3c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sim"), 0x223c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simdot"),
						 0x2a6a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sime"),
						 0x2243);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simeq"),
						 0x2243);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simg"),
						 0x2a9e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simgE"),
						 0x2aa0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("siml"),
						 0x2a9d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simlE"),
						 0x2a9f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simne"),
						 0x2246);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simplus"),
						 0x2a24);
	hb_map_entity_references_set_whole_array(map, nh_litarr("simrarr"),
						 0x2972);
	hb_map_entity_references_set_whole_array(map, nh_litarr("slarr"),
						 0x2190);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("smallsetminus"), 0x2216);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smashp"),
						 0x2a33);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smeparsl"),
						 0x29e4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smid"),
						 0x2223);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smile"),
						 0x2323);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smt"), 0x2aaa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("smte"),
						 0x2aac);
	hb_map_entity_references_set_whole_array(map, nh_litarr("softcy"),
						 0x44c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sol"), 0x2f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("solb"),
						 0x29c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("solbar"),
						 0x233f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sopf"),
						 0x1d564);
	hb_map_entity_references_set_whole_array(map, nh_litarr("spades"),
						 0x2660);
	hb_map_entity_references_set_whole_array(map, nh_litarr("spadesuit"),
						 0x2660);
	hb_map_entity_references_set_whole_array(map, nh_litarr("spar"),
						 0x2225);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqcap"),
						 0x2293);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqcup"),
						 0x2294);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsub"),
						 0x228f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsube"),
						 0x2291);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsubset"),
						 0x228f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsubseteq"),
						 0x2291);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsup"),
						 0x2290);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsupe"),
						 0x2292);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsupset"),
						 0x2290);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sqsupseteq"),
						 0x2292);
	hb_map_entity_references_set_whole_array(map, nh_litarr("squ"), 0x25a1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("square"),
						 0x25a1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("squarf"),
						 0x25aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("squf"),
						 0x25aa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("srarr"),
						 0x2192);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sscr"),
						 0x1d4c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ssetmn"),
						 0x2216);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ssmile"),
						 0x2323);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sstarf"),
						 0x22c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("star"),
						 0x2606);
	hb_map_entity_references_set_whole_array(map, nh_litarr("starf"),
						 0x2605);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("straightepsilon"), 0x3f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("straightphi"),
						 0x3d5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("strns"), 0xaf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sub"), 0x2282);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subE"),
						 0x2ac5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subdot"),
						 0x2abd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sube"),
						 0x2286);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subedot"),
						 0x2ac3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("submult"),
						 0x2ac1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subnE"),
						 0x2acb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subne"),
						 0x228a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subplus"),
						 0x2abf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subrarr"),
						 0x2979);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subset"),
						 0x2282);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subseteq"),
						 0x2286);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subseteqq"),
						 0x2ac5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subsetneq"),
						 0x228a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subsetneqq"),
						 0x2acb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subsim"),
						 0x2ac7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subsub"),
						 0x2ad5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("subsup"),
						 0x2ad3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succ"),
						 0x227b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succapprox"),
						 0x2ab8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succcurlyeq"),
						 0x227d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succeq"),
						 0x2ab0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succnapprox"),
						 0x2aba);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succneqq"),
						 0x2ab6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succnsim"),
						 0x22e9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("succsim"),
						 0x227f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sum"), 0x2211);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sung"),
						 0x266a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sup"), 0x2283);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sup1"), 0xb9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sup2"), 0xb2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("sup3"), 0xb3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supE"),
						 0x2ac6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supdot"),
						 0x2abe);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supdsub"),
						 0x2ad8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supe"),
						 0x2287);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supedot"),
						 0x2ac4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("suphsub"),
						 0x2ad7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("suplarr"),
						 0x297b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supmult"),
						 0x2ac2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supnE"),
						 0x2acc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supne"),
						 0x228b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supplus"),
						 0x2ac0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supset"),
						 0x2283);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supseteq"),
						 0x2287);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supseteqq"),
						 0x2ac6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supsetneq"),
						 0x228b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supsetneqq"),
						 0x2acc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supsim"),
						 0x2ac8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supsub"),
						 0x2ad4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("supsup"),
						 0x2ad6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("swArr"),
						 0x21d9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("swarhk"),
						 0x2926);
	hb_map_entity_references_set_whole_array(map, nh_litarr("swarr"),
						 0x2199);
	hb_map_entity_references_set_whole_array(map, nh_litarr("swarrow"),
						 0x2199);
	hb_map_entity_references_set_whole_array(map, nh_litarr("swnwar"),
						 0x292a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("szlig"), 0xdf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("target"),
						 0x2316);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tau"), 0x3c4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tbrk"),
						 0x23b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tcaron"),
						 0x165);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tcedil"),
						 0x163);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tcy"), 0x442);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tdot"),
						 0x20db);
	hb_map_entity_references_set_whole_array(map, nh_litarr("telrec"),
						 0x2315);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tfr"),
						 0x1d531);
	hb_map_entity_references_set_whole_array(map, nh_litarr("there4"),
						 0x2234);
	hb_map_entity_references_set_whole_array(map, nh_litarr("therefore"),
						 0x2234);
	hb_map_entity_references_set_whole_array(map, nh_litarr("theta"),
						 0x3b8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thetasym"),
						 0x3d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thetav"),
						 0x3d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thickapprox"),
						 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thicksim"),
						 0x223c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thinsp"),
						 0x2009);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thkap"),
						 0x2248);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thksim"),
						 0x223c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("thorn"), 0xfe);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tilde"),
						 0x2dc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("times"), 0xd7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("timesb"),
						 0x22a0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("timesbar"),
						 0x2a31);
	hb_map_entity_references_set_whole_array(map, nh_litarr("timesd"),
						 0x2a30);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tint"),
						 0x222d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("toea"),
						 0x2928);
	hb_map_entity_references_set_whole_array(map, nh_litarr("top"), 0x22a4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("topbot"),
						 0x2336);
	hb_map_entity_references_set_whole_array(map, nh_litarr("topcir"),
						 0x2af1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("topf"),
						 0x1d565);
	hb_map_entity_references_set_whole_array(map, nh_litarr("topfork"),
						 0x2ada);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tosa"),
						 0x2929);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tprime"),
						 0x2034);
	hb_map_entity_references_set_whole_array(map, nh_litarr("trade"),
						 0x2122);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triangle"),
						 0x25b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triangledown"),
						 0x25bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triangleleft"),
						 0x25c3);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("trianglelefteq"), 0x22b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triangleq"),
						 0x225c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("triangleright"), 0x25b9);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("trianglerighteq"), 0x22b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tridot"),
						 0x25ec);
	hb_map_entity_references_set_whole_array(map, nh_litarr("trie"),
						 0x225c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triminus"),
						 0x2a3a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("triplus"),
						 0x2a39);
	hb_map_entity_references_set_whole_array(map, nh_litarr("trisb"),
						 0x29cd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tritime"),
						 0x2a3b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("trpezium"),
						 0x23e2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tscr"),
						 0x1d4c9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tscy"), 0x446);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tshcy"),
						 0x45b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("tstrok"),
						 0x167);
	hb_map_entity_references_set_whole_array(map, nh_litarr("twixt"),
						 0x226c);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("twoheadleftarrow"), 0x219e);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("twoheadrightarrow"), 0x21a0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uArr"),
						 0x21d1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uHar"),
						 0x2963);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uacute"),
						 0xfa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uarr"),
						 0x2191);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ubrcy"),
						 0x45e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ubreve"),
						 0x16d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ucirc"), 0xfb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ucy"), 0x443);
	hb_map_entity_references_set_whole_array(map, nh_litarr("udarr"),
						 0x21c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("udblac"),
						 0x171);
	hb_map_entity_references_set_whole_array(map, nh_litarr("udhar"),
						 0x296e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ufisht"),
						 0x297e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ufr"),
						 0x1d532);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ugrave"),
						 0xf9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uharl"),
						 0x21bf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uharr"),
						 0x21be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uhblk"),
						 0x2580);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ulcorn"),
						 0x231c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ulcorner"),
						 0x231c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ulcrop"),
						 0x230f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ultri"),
						 0x25f8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("umacr"),
						 0x16b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uml"), 0xa8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uogon"),
						 0x173);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uopf"),
						 0x1d566);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uparrow"),
						 0x2191);
	hb_map_entity_references_set_whole_array(map, nh_litarr("updownarrow"),
						 0x2195);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("upharpoonleft"), 0x21bf);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("upharpoonright"), 0x21be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uplus"),
						 0x228e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("upsi"), 0x3c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("upsih"),
						 0x3d2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("upsilon"),
						 0x3c5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("upuparrows"),
						 0x21c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("urcorn"),
						 0x231d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("urcorner"),
						 0x231d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("urcrop"),
						 0x230e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uring"),
						 0x16f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("urtri"),
						 0x25f9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uscr"),
						 0x1d4ca);
	hb_map_entity_references_set_whole_array(map, nh_litarr("utdot"),
						 0x22f0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("utilde"),
						 0x169);
	hb_map_entity_references_set_whole_array(map, nh_litarr("utri"),
						 0x25b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("utrif"),
						 0x25b4);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uuarr"),
						 0x21c8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uuml"), 0xfc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("uwangle"),
						 0x29a7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vArr"),
						 0x21d5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vBar"),
						 0x2ae8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vBarv"),
						 0x2ae9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vDash"),
						 0x22a8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vangrt"),
						 0x299c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varepsilon"),
						 0x3b5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varkappa"),
						 0x3f0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varnothing"),
						 0x2205);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varphi"),
						 0x3c6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varpi"),
						 0x3d6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varpropto"),
						 0x221d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varr"),
						 0x2195);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varrho"),
						 0x3f1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("varsigma"),
						 0x3c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vartheta"),
						 0x3d1);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("vartriangleleft"), 0x22b2);
	hb_map_entity_references_set_whole_array(
		map, nh_litarr("vartriangleright"), 0x22b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vcy"), 0x432);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vdash"),
						 0x22a2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vee"), 0x2228);
	hb_map_entity_references_set_whole_array(map, nh_litarr("veebar"),
						 0x22bb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("veeeq"),
						 0x225a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vellip"),
						 0x22ee);
	hb_map_entity_references_set_whole_array(map, nh_litarr("verbar"),
						 0x7c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vert"), 0x7c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vfr"),
						 0x1d533);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vltri"),
						 0x22b2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vopf"),
						 0x1d567);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vprop"),
						 0x221d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vrtri"),
						 0x22b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vscr"),
						 0x1d4cb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("vzigzag"),
						 0x299a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wcirc"),
						 0x175);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wedbar"),
						 0x2a5f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wedge"),
						 0x2227);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wedgeq"),
						 0x2259);
	hb_map_entity_references_set_whole_array(map, nh_litarr("weierp"),
						 0x2118);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wfr"),
						 0x1d534);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wopf"),
						 0x1d568);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wp"), 0x2118);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wr"), 0x2240);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wreath"),
						 0x2240);
	hb_map_entity_references_set_whole_array(map, nh_litarr("wscr"),
						 0x1d4cc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xcap"),
						 0x22c2);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xcirc"),
						 0x25ef);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xcup"),
						 0x22c3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xdtri"),
						 0x25bd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xfr"),
						 0x1d535);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xhArr"),
						 0x27fa);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xharr"),
						 0x27f7);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xi"), 0x3be);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xlArr"),
						 0x27f8);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xlarr"),
						 0x27f5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xmap"),
						 0x27fc);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xnis"),
						 0x22fb);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xodot"),
						 0x2a00);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xopf"),
						 0x1d569);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xoplus"),
						 0x2a01);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xotime"),
						 0x2a02);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xrArr"),
						 0x27f9);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xrarr"),
						 0x27f6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xscr"),
						 0x1d4cd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xsqcup"),
						 0x2a06);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xuplus"),
						 0x2a04);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xutri"),
						 0x25b3);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xvee"),
						 0x22c1);
	hb_map_entity_references_set_whole_array(map, nh_litarr("xwedge"),
						 0x22c0);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yacute"),
						 0xfd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yacy"), 0x44f);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ycirc"),
						 0x177);
	hb_map_entity_references_set_whole_array(map, nh_litarr("ycy"), 0x44b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yen"), 0xa5);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yfr"),
						 0x1d536);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yicy"), 0x457);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yopf"),
						 0x1d56a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yscr"),
						 0x1d4ce);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yucy"), 0x44e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("yuml"), 0xff);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zacute"),
						 0x17a);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zcaron"),
						 0x17e);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zcy"), 0x437);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zdot"), 0x17c);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zeetrf"),
						 0x2128);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zeta"), 0x3b6);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zfr"),
						 0x1d537);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zhcy"), 0x436);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zigrarr"),
						 0x21dd);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zopf"),
						 0x1d56b);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zscr"),
						 0x1d4cf);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zwj"), 0x200d);
	hb_map_entity_references_set_whole_array(map, nh_litarr("zwnj"),
						 0x200c);
}

void hb_rule_entity_reference_init(void)
{
	hb_rule_entity_reference_map = hb_map_entity_references_create();
	hb_rule_entity_reference_map_add_entries(hb_rule_entity_reference_map);
}

bool hb_rule_entity_reference_valid_name_char(hb_rune c)
{
	return hb_rule_ascii_digit_check(c) || hb_rule_ascii_uppercase_check(c)
	       || hb_rule_ascii_lowercase_check(c);
}

bool hb_rule_entity_reference_exists(nh_view_str* ref)
{
	return hb_map_entity_references_has(hb_rule_entity_reference_map, ref);
}

int32_t hb_rule_entity_reference_get_code_point(nh_view_str* ref)
{
	return hb_map_entity_references_get(hb_rule_entity_reference_map, ref);
}
