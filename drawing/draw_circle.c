/**************************************************************************
 * circle.c                                                               *
 * written by David Brackeen                                              *
 * http://www.brackeen.com/home/vga/                                      *
 *                                                                        *
 * This is a 16-bit program.                                              *
 * Tab stops are set to 2.                                                *
 * Remember to compile in the LARGE memory model!                         *
 * To compile in Borland C: bcc -ml circle.c                              *
 *                                                                        *
 * This program will only work on DOS- or Windows-based systems with a    *
 * VGA, SuperVGA or compatible video adapter.                             *
 *                                                                        *
 * Please feel free to copy this source code.                             *
 *                                                                        *
 * DESCRIPTION: This program demostrates drawing how much faster it is to *
 * draw circles using tables rather than math functions.                  *
 **************************************************************************/
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <floatmap.h>

/* To avoid the floating point operation that is the sin of arccos,
 * prepare the table for sin of arccos maked by below code:
 * 	int i;
 *	FILE *fd = fopen("sin_acos.tbl", "wb");
 *	fprintf(fd, "const signed long SIN_OF_ACOS[1024] = {");
 *	for (i = 0; i < 1024; i++) {
 *		if ((i%32) == 0) fprintf(fd, "\n\t");
 *		fprintf(fd, "%d, ", sin(acos((float)i/1024))*0x10000L);
 *	}
 *	fprintf(fd, "\n};\n");
 *	fclose(fd);
 */
const signed long SIN_OF_ACOS[1024] = {
  0, -1024, -16384, -82944, -262146, -640008, -1327127, -2458681, -4194432, -6718724, -10240488, -14993249, -21235122, -29248821, -39341661, -51845563, -67117057, -85537292, -107512035, -133471681, -163871257, -199190433, -239933521, -286629490, -339831968, -400119254, -468094323, -544384836, -629643152, -724546331, -829796148, -946119104, 
  -1074266432, -1215014113, -1369162883, -1537538247, -1720990488, -1920394683, -2136650714, 1924284016, 1671525387, 1399066319, 1105907579, 791025063, 453369776, 91867822, -294579612, -707096277, -1146830870, -1614957057, -2112673486, 1653763487, 1093170603, 499241452, -129322726, -793845765, -1495676591, 2058778051, 1278184400, 456085428, -408968363, -1318451662, 2021102921, 1018235651, 
  -33636577, -1136089446, 2004243351, 795800908, -468101146, -1789139851, 1125949647, -314495109, -1817260135, 910843074, -721924363, 1872542330, 102453192, -1739105432, 640927195, -1349315711, 878142583, -1268616064, 798333668, -1512977835, 385323217, -2098784382, -377478493, 1252135262, -1507074643, -67363781, 1274084422, -1779939462, -641769005, 391334269, 1317050031, 2133031817, 
  -1458060299, -868690558, -396251328, -43193956, 188003902, 294838243, 274778679, 125268403, -156275852, -572463851, -1125931894, -1819342855, 1639581071, 658189151, -471294158, -1751638429, 1109327285, -481154776, 2063999953, 151979950, -1925151364, 124642444, 2003436181, -586722422, 941089134, -2006103002, -841430712, 137044981, 926203042, 1522895088, 1923945336, 2126150557, 
  2126280031, 1921075490, 1507251080, 881493302, 40460967, -1019214857, 1994036403, 486855588, -1249242471, 1077229111, -1127172671, 723950308, -1962901119, -601385097, 509910123, 1367335316, 1967212927, -1989130276, -1915494079, -2110608655, 1716701863, 972682840, -51547144, -1359865534, 1338788729, -549454033, 1561376774, -922646410, 584389004, 1783464620, -1624467113, -1053581081, 
  -802983344, -876842012, -1279354612, -2014748148, 1207688127, -206266538, -1965960685, 219227400, 2049955027, -773150378, 335410554, 1076205279, 1444738465, 1436484616, 1046888007, 271362610, -894707978, 1838996688, -122135355, 1807122518, -967902921, 137953542, 824924409, 1088178585, 922854000, 324057537, -713135052, 2101288076, 172405589, 2085133974, -755510910, 235324451, 
  757560077, 806051592, 375622849, -538934156, -1942859367, 453542543, -1944967097, -553789969, 326738075, 691216109, 534210752, -149743924, -1366146482, 1174439091, -1123485898, 324416139, 1217547905, 1550246310, 1316815082, 511524674, -871387831, 1457248482, -1098330920, 45944956, 589211462, 525537397, -151042393, -1446527108, 928017199, -1623411663, -516981050, -53794658, 
  -240023444, -1081873018, 1709383539, -462463608, 986209921, 1754091534, 1834800753, 1221921804, -90996487, -2110440808, -547966170, 294902089, 411570084, -204592025, -1560250152, 632860906, 2073035460, -1541468594, -1627528002, 1808009252, 168323824, 2036428672, -1184569435, -911731670, -1447058336, 1497347484, -675556668, 617018317, 1072921921, 684933058, -554207350, 1643170798, 
  -1320202534, -861766572, -1283900936, 1700910710, -504755955, 681505032, 957158713, 314598254, -1253822479, 539178234, 1390907941, 1293601775, 239455011, -1779377070, -475812026, -152742287, -818133405, 1814975797, -851396100, -235402189, -640138449, -2073774719, -249554838, 529301314, 254499552, -1082296063, 805503923, 1614512312, 1336266988, -37736538, 1778922015, -1812281646, 
  2064921818, 516954964, 2125033665, -1709538727, 1889334129, 27901140, 1287203863, 1363337977, 247322296, -2069868698, -1302337728, -1754166783, 860451895, -2057620846, -1927700169, 1240918797, -1151039216, -523026027, -1179441249, 1165204006, -2088549539, 1934628862, 340219539, 1708492466, 1734768715, 409289446, 2017215719, -2041240886, 1108919912, -1427153361, -1069523873, -2123205043, 
  -303324274, 85007677, -968401623, 821173518, 1148474827, 3161742, 1669811211, 1843015594, 512284341, 1962043782, 1886735022, 275715622, 1413259380, 993654220, -993896125, -265272348, -1126341666, 706911132, 928514283, -472589292, 787457785, 402525232, -1638603237, -1052230146, -2144646374, -632262213, -821476789, 1571190888, -2055734804, 1171052033, -1645001690, -1925668034, 
  317290645, 777088744, -558148244, 594616009, -71573469, 1726205975, 1680885175, -219695225, 307214965, -1045626669, 4413772, -850022953, 673579694, 267744568, -2080097543, -2087608188, 232523867, 572582370, -1080242112, -443852477, -1826147259, -945151734, -2108887393, -1035503047, -2033144311, -820084711, -1704595165, -405075106, -1229921962, 102337719, -716821217, 593946337, 
  -274013736, 960514084, -11254541, 1091765058, -39342949, 876373974, -470132786, 201955870, -1416542764, -1044944344, 1302400297, 1316104816, -1018318394, -1420458539, 95058636, -781429505, 230279058, -1179618543, -731061233, 1560973763, 1386470555, -1269691181, -2127736566, -1202930486, 1489389340, 1638844687, -770048962, -1457882795, -440289644, -2027944374, -1941662378, -197301862, 
  -1105764238, -388092628, 1939625839, 1566259631, -1524433003, 1241162849, 1256714581, -1494254579, 1561634021, 1817810250, -742441448, -1840949782, -1494591639, 279674781, -830157774, -546244158, 1114210836, -161047970, -94424703, 1296625213, -300405395, -608174030, 355609124, -1721819272, 1731593006, 2107942186, -610828197, 2147072209, 1773476311, -1749937148, 148355918, -1140079341, 
  -1338866142, -466685763, 1457689242, 120427233, -202461153, 469974588, 2118591596, 429185812, -322606663, -156212004, 908848089, -1442011452, 1361429566, 709424961, 877032689, 1844245092, -704011680, 1801990946, 752012304, 420612979, 787285028, 1831418684, -762665547, 1574151697, 231016948, -518125636, -694404075, -319051900, 586591221, 2001078838, -392110212, 1975296729, 
  491594415, -570129890, -1231866260, -1515715478, -1443889712, -1038713198, -322622922, 681830685, 1951983053, -829912534, 903183548, -1461674535, 642319577, -1398014661, 983892938, -825375816, 1740510131, 67891955, -1572108728, 1091507476, -555284914, 2053234034, 302790515, -1536112736, 806900869, -1282820922, 759811057, -1680111800, -37758327, 1366670502, -1787157374, -934804072, 
  -396866390, -199107495, -367425087, -927852278, -1906558483, 965116984, -929205178, 973825901, -1942448020, -1114956862, -865674221, -1221749140, 2084492941, 435680048, -1900803536, -657720123, -157914213, -429411590, -1500387634, 895798968, -1859263995, -1204272210, -1462976960, 1630650302, -542419584, 578496510, 669021848, -300412075, 1935433360, -1243266397, -1276629115, 1805129306, 
  -618306306, 12452227, -628275807, 1723595297, -1552920006, -1899108641, 653636105, 1778780658, 1444584515, -380868297, 565296819, -44158124, 2053284205, -1764941318, 1353254792, -1510024946, -1798026051, 455885501, 923189221, -429856671, 657783367, -142981079, 1428501391, 1042753383, -1334930835, -1444486792, 678984132, 705213227, -1401302455, -1381301606, 729305220, 599434067, 
  -1807239388, 2062685892, -712436355, -1579628337, -576060891, -2034087831, -1696345704, 399342647, -80033598, 1122226946, -327333031, -172462093, 1547896303, 499602032, 938218397, -1470858866, 1822433065, -2106916121, -414351474, -1730394932, -1800908855, -666967137, 1630110145, 753787466, 957213757, -2096648795, 139809713, -965926231, -1161728554, -490696874, 1003806675, -1016811993, 
  1993488107, 1400609768, 1455086456, 2112210892, -967967210, 759225761, -1341686065, 1273407990, -31535569, -1007945726, -1702508546, 2132764977, 1860667274, 1728595485, 1688679754, 1692747980, 1692323240, 1638621186, 1482547410, 1174694780, 665340747, -95555376, -1158355173, 1721223387, -97766050, 1923269994, -857276413, 98520830, 443347841, 124516957, -911003583, 1578374606, 
  -1051012214, -263311961, -407930961, -1539667965, 581282618, 1604425559, 1473863383, 133325726, 1821133236, -2109675038, 1168393245, -1287360050, -945186831, 2136332546, -691714363, -898772001, 1455375685, 2015568761, 721202078, 1806221401, 914217606, -2016675398, 1541182617, -1359836942, 2102004657, -1021798249, 2089607623, -1513177325, 989797661, 943189052, -1718874323, 1527204234, 
  2024680969, -293729153, -1200824399, -764855591, 945436416, -434151407, -678387285, 142488511, 1957725919, 401092697, -304228859, -230549034, 549291300, 1961915310, -361562531, 2094327216, 664632307, -431255785, -1269476785, -1926740140, 1814636389, 1286851565, 706418402, -5711155, -929185156, -2144257945, 563170743, -1478319878, 239092858, 1337697343, 1734110124, 1344301769, 
  83589923, -2133367723, -1097613276, -1190797761, 1794680724, -819166913, -531157232, -1725712988, -198034419, -333976298, 2069813254, -1668951810, 1241534678, 2117489806, 864304185, 1681566069, 178150757, 552080932, -1589351495, -2049725301, -928401082, 1674436665, 1362802727, -1965163892, 177757517, -901944363, -1013751911, -262998011, 1244090828, -894583402, 1802868235, 637539645, 
  -205510302, -837138199, -1369160231, -1914363773, 1708448120, 794575560, -476786550, 2072537236, -265211525, 981033158, 1396377686, 859824889, -750706646, 736571133, 902400041, -378636499, 1061872276, 801248080, -1289389281, -1045136901, 1402741402, 1626799590, -506671759, -837661464, 497611374, -933319191, -974280913, 234617629, -1743030408, 1539921331, 1349383286, 1834782703, 
  -1445791510, -50771308, 1575067392, -1014513076, 617669739, 2022394261, -1251083974, -770151533, -988662361, -2067090467, 127447791, 1136269141, 794002186, -1066403413, -318733594, -1428436184, -272776566, -1320732176, -453149117, -2142637613, 2021237693, -1027807910, 1411833253, 564992652, 539435036, 1145959818, -2101628943, -806683944, 540450384, 1742275261, -1695829474, -1385769253, 
  -1826572297, 1070412457, -1493356235, -1138876659, 1920555537, -1120689940, -1890812300, -610422278, -1797614469, -1383105898, 404820794, -959724814, -1415382090, -1198485201, -548140711, 293726558, 1082326688, 1569956813, 1505952231, 636636453, -1294729841, -253033675, -796264987, 1104298057, 884164153, -1729526222, 1576908233, 1933826294, -941992950, 1252587586, -362804558, -1787345549, 
  976016102, -964410661, 675641721, 1291556779, 569629937, -1807973798, -1868329356, 62235134, -641942227, -21037766, 1585271310, -462279737, 2077237208, 260081024, -1977480863, -704170025, -583729693, -1990123447, -1007638370, 1979074538, -2010075490, -485959323, 1854952093, 310339570, -1238150180, 1085037180, -1735665273, -1542305091, 1226651784, 1831150661, -180658758, -972567743, 
  -1010392557, -767160771, -723269227, -1366646479, 1102047405, 1884347053, 468706790, 630318593, 1840990109, -731048274, 958527092, 2060124720, 2009896384, 234552642, 446106305, 2051707135, 153403988, -1567198584, 560872090, 1607761209, 927419082, -2137644132, 333289228, -1861911480, -362995581, -544071548, 451453195, 1158584422, 85010365, 5347233, -629541963, 896614821, 
  -1320202535, -330297189, -2100915788, 252668091, 696431025, 1751483749, 1608181985, -1579929946, -1107246257, 1102929611, -1207790862, -1454198803, -1683556755, 308023095, -1910653160, -1932407330, -1987963327, -62540735, 1512654528, 353777792, -1682373638, 1499882684, -1241552846, 366235353, -645517829, 1575436540, -69609393, 137220833, -746811990, -1442630736, -746891425, -1829651312, 
  647706869, -945320242, -1441651733, -59738906, -406969812, -1893305806, 264970573, -2138548981, -239762019, 1826827445, -191560332, -2080856804, 244854763, -2146899772, -1147841211, -1679644127, -225366602, -2015938585, 437677855, 1567283771, -80521648, -1852639847, -1293237725, -448048370, -1579313590, 1413620328, 1508267064, 21916269, -1991000569, 542515390, -480292227, -583572382, 
  90751427, 1060271712, 1483154308, 137817098, -303927283, -1890272231, 1470223371, -1788395998, 2030806678, 316300575, 1358367585, -47869192, -1168015018, 37745938, 571877234, 937298728, 785486152, -1145822573, 1489022615, 1093751893, 1817448378, -2011301732, -215192167, -1239355997, 2084655462, -1958580278, -1172382200, 1725318050, 1793814864, 242208566, -111052906, 588645761, 
  2126280030, 1375397017, -1846218705, -194432769, -787743224, -719114499, -1438155971, -1920191850, -745199035, 1429167552, -1094906297, -1156405466, -267358191, -1072555648, -1093545224, -1040503843, 1818952144, 91216197, 1488632410, 1002378570, 577507993, -642561482, -264724810, 1496954564, -33636577, -1402883516, -2035212388, 581611347, -1984734877, 556159877, -1074266432, 1363660690, 
};

void bitmap_draw_circle(int x, int y, int r, uint8_t val, bitmap_t *m)
{
  real_t n = 0;
  real_t inv_r;
  int dx, dy, w, h;

  assert(m);
  assert(r > 1);

  w = bitmap_get_width(m);
  h = bitmap_get_height(m);

  inv_r = 1.0 / r;
  dx = 0, dy = r - 1;
  while (dx <= dy) {
    /* octant 0 */
    if ((((x + dy) >= 0) && ((x + dy) < w)) &&
	(((y - dx) >= 0) && ((y - dx) < h))) {
      if (val != 0) bitmap_set_value(x + dy, y - dx, m);
      else bitmap_reset_value(x + dy, y - dx, m);
      //*(p->buffer+(y - dx)*p->header.pitch+(x + dy)) = col;
    }
    /* octant 1 */
    if ((((x + dx) >= 0) && ((x + dx) < w)) &&
	(((y - dy) >= 0) && ((y - dy) < h))) {
      if (val != 0) bitmap_set_value(x + dx, y - dy, m);
      else bitmap_reset_value(x + dx, y - dy, m);
      //*(p->buffer+(y - dy)*p->header.pitch+(x + dx)) = col;
    }
    /* octant 2 */
    if ((((x - dx) >= 0) && ((x - dx) < w)) &&
	(((y - dy) >= 0) && ((y - dy) < h))) {
      if (val != 0) bitmap_set_value(x - dx, y - dy, m);
      else bitmap_reset_value(x - dx, y - dy, m);
      //*(m->buffer+(y - dy)*m->header.pitch+(x - dx)) = col;
    }
    /* octant 3 */
    if ((((x - dy) >= 0) && ((x - dy) < w)) &&
	(((y - dx) >= 0) && ((y - dx) < h))) {
      if (val != 0) bitmap_set_value(x - dy, y - dx, m);
      else bitmap_reset_value(x - dy, y - dx, m);
      //*(m->buffer+(y - dx)*m->header.pitch+(x - dy)) = col;
    }
    /* octant 4 */
    if ((((x - dy) >= 0) && ((x - dy) < w)) &&
	(((y + dx) >= 0) && ((y + dx) < h))) {
      if (val != 0) bitmap_set_value(x - dy, y + dx, m);
      else bitmap_reset_value(x - dy, y + dx, m);
      //*(m->buffer+(y + dx)*m->header.pitch+(x - dy)) = col;
    }
    /* octant 5 */
    if ((((x - dx) >= 0) && ((x - dx) < w)) &&
	(((y + dy) >= 0) && ((y + dy) < h))) {
      if (val != 0) bitmap_set_value(x - dx, y + dy, m);
      else bitmap_reset_value(x - dx, y + dy, m);
      //*(m->buffer+(y + dy)*m->header.pitch+(x - dx)) = col;
    }
    /* octant 6 */
    if ((((x + dx) >= 0) && ((x + dx) < w)) &&
	(((y + dy) >= 0) && ((y + dy) < h))) {
      if (val != 0) bitmap_set_value(x + dx, y + dy, m);
      else bitmap_reset_value(x + dx, y + dy, m);
      //*(m->buffer+(y + dy)*m->header.pitch+(x + dx)) = col;
    }
    /* octant 7 */
    if ((((x + dy) >= 0) && ((x + dy) < w)) &&
	(((y + dx) >= 0) && ((y + dx) < h))) {
      if (val != 0) bitmap_set_value(x + dy, y + dx, m);
      else bitmap_reset_value(x + dy, y + dx, m);
      //*(m->buffer+(y + dx)*m->header.pitch+(x + dy)) = col;
    }
    dx++;
    n += inv_r;
    dy = (int)(r * sin(acos(n)));
  }
}

#define PIXM_DRAW_CIRCLE(type, x, y, r, v, m) {         \
    real_t n = 0;                                       \
    real_t inv_r;                                       \
    int dx, dy, w, h, pitch;                            \
    type *buf;                                          \
    assert(m);                                          \
    assert(r > 1);                                      \
    w = (m)->header.width;                              \
    h = (m)->header.height;                             \
    pitch = (m)->header.pitch / sizeof(*buf);           \
    buf = (m)->buffer;                                  \
    inv_r = 1.0 / (r);                                  \
    dx = 0, dy = r - 1;                                 \
    while (dx <= dy) {                                  \
      if ((((x + dy) >= 0) && ((x + dy) < w)) &&        \
	  (((y - dx) >= 0) && ((y - dx) < h))) {        \
	*(buf + (y - dx) * pitch + (x + dy)) = v;	\
      }                                                 \
      if ((((x + dx) >= 0) && ((x + dx) < w)) &&        \
	  (((y - dy) >= 0) && ((y - dy) < h))) {        \
	*(buf + (y - dy) * pitch + (x + dx)) = v;	\
      }                                                 \
      if ((((x - dx) >= 0) && ((x - dx) < w)) &&        \
	  (((y - dy) >= 0) && ((y - dy) < h))) {        \
	*(buf + (y - dy) * pitch + (x - dx)) = v;	\
      }                                                 \
      if ((((x - dy) >= 0) && ((x - dy) < w)) &&        \
	  (((y - dx) >= 0) && ((y - dx) < h))) {        \
	*(buf + (y - dx) * pitch + (x - dy)) = v;	\
      }                                                 \
      if ((((x - dy) >= 0) && ((x - dy) < w)) &&        \
	  (((y + dx) >= 0) && ((y + dx) < h))) {        \
	*(buf + (y + dx) * pitch + (x - dy)) = v;	\
      }                                                 \
      if ((((x - dx) >= 0) && ((x - dx) < w)) &&        \
	  (((y + dy) >= 0) && ((y + dy) < h))) {        \
	*(buf + (y + dy) * pitch + (x - dx)) = v;	\
      }                                                 \
      if ((((x + dx) >= 0) && ((x + dx) < w)) &&        \
	  (((y + dy) >= 0) && ((y + dy) < h))) {        \
	*(buf + (y + dy) * pitch + (x + dx)) = v;	\
      }                                                 \
      if ((((x + dy) >= 0) && ((x + dy) < w)) &&        \
	  (((y + dx) >= 0) && ((y + dx) < h))) {        \
	*(buf + (y + dx) * pitch + (x + dy)) = v;	\
      }                                                 \
      dx++;                                             \
      n += inv_r;                                       \
      dy = (int)(r * sin(acos(n)));                     \
    }                                                   \
  }

void bytem_draw_circle(int xorg, int yorg, int radius, uint8_t val, bytemap_t *m)
{
  PIXM_DRAW_CIRCLE(uint8_t, xorg, yorg, radius, val, m);
}

void wordm_draw_circle(int xorg, int yorg, int radius, uint16_t val, wordmap_t *m)
{
  PIXM_DRAW_CIRCLE(uint16_t, xorg, yorg, radius, val, m);
}

void dwordm_draw_circle(int xorg, int yorg, int radius, uint32_t val, dwordmap_t *m)
{
  PIXM_DRAW_CIRCLE(uint32_t, xorg, yorg, radius, val, m);
}

void floatm_draw_circle(int xorg, int yorg, int radius, float val, floatmap_t *m)
{
  PIXM_DRAW_CIRCLE(float, xorg, yorg, radius, val, m);
}

void bitmap_draw_filled_circle(int xorg, int yorg, int radius, uint8_t val, bitmap_t *m)
{
  real_t n = 0;
  real_t inv_r;
  int dx, dy, i, w, h;

  assert(m);
  assert(radius > 1.0);

  w = bitmap_get_width(m);
  h = bitmap_get_height(m);

  inv_r = 1.0 / (real_t)radius;

  dx = 0;
  dy = radius - 1;

  while (dx <= dy) {
    for (i = dy; i >= dx; i--) {
      /* octant 0 */
      if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&
	  (((yorg - dx) >= 0) && ((yorg - dx) < h))) {
	if (val != 0) bitmap_set_value(xorg + i, yorg - dx, m);
	else bitmap_reset_value(xorg + i, yorg - dx, m);
	//*(m->buffer+(yorg - dx)*m->header.pitch+(xorg + i)) = val;
      }
      /* octant 1 */
      if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&
	  (((yorg - i) >= 0) && ((yorg - i) < h))) {
	if (val != 0) bitmap_set_value(xorg + dx, yorg - i, m);
	else bitmap_reset_value(xorg + dx, yorg - i, m);
	//*(m->buffer+(yorg - i)*m->header.pitch+(xorg + dx)) = val;
      }
      /* octant 2 */
      if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&
	  (((yorg - i) >= 0) && ((yorg - i) < h))) {
	if (val != 0) bitmap_set_value(xorg - dx, yorg - i, m);
	else bitmap_reset_value(xorg - dx, yorg - i, m);
	//*(m->buffer+(yorg - i)*m->header.pitch+(xorg - dx)) = val;
      }
      /* octant 3 */
      if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&
	  (((yorg - dx) >= 0) && ((yorg - dx) < h))) {
	if (val != 0) bitmap_set_value(xorg - i, yorg - dx, m);
	else bitmap_reset_value(xorg - i, yorg - dx, m);
	//*(m->buffer+(yorg - dx)*m->header.pitch+(xorg - i)) = val;
      }
      /* octant 4 */
      if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&
	  (((yorg + dx) >= 0) && ((yorg + dx) < h))) {
	if (val != 0) bitmap_set_value(xorg - i, yorg + dx, m);
	else bitmap_reset_value(xorg - i, yorg + dx, m);
	//*(m->buffer+(yorg + dx)*m->header.pitch+(xorg - i)) = val;
      }
      /* octant 5 */
      if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&
	  (((yorg + i) >= 0) && ((yorg + i) < h))) {
	if (val != 0) bitmap_set_value(xorg - dx, yorg + i, m);
	else bitmap_reset_value(xorg - dx, yorg + i, m);
	//*(m->buffer+(yorg + i)*m->header.pitch+(xorg - dx)) = val;
      }
      /* octant 6 */
      if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&
	  (((yorg + i) >= 0) && ((yorg + i) < h))) {
	if (val != 0) bitmap_set_value(xorg + dx, yorg + i, m);
	else bitmap_reset_value(xorg + dx, yorg + i, m);
	//*(m->buffer+(yorg + i)*m->header.pitch+(xorg + dx)) = val;
      }
      /* octant 7 */
      if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&
	  (((yorg + dx) >= 0) && ((yorg + dx) < h))) {
	if (val != 0) bitmap_set_value(xorg + i, yorg + dx, m);
	else bitmap_reset_value(xorg + i, yorg + dx, m);
	//*(m->buffer+(yorg + dx)*m->header.pitch+(xorg + i)) = val;
      }
    }
    dx++;
    n += inv_r;
    dy = radius * sin(acos(n));
  }
}

#define PIXM_DRAW_FILLED_CIRCLE(type, xorg, yorg, r, val, m) {	\
    real_t n = 0;                                               \
    real_t inv_r;                                               \
    int dx, dy, i, w, h, pitch;                                 \
    type *buf;                                                  \
    assert(m);							\
    assert(r > 1.0);                                            \
    w = (m)->header.width;                                      \
    h = (m)->header.height;                                     \
    pitch = (m)->header.pitch / sizeof(*buf);                   \
    buf = (m)->buffer;						\
    inv_r = 1.0 / (r);                                          \
    dx = 0;                                                     \
    dy = (r) - 1;                                               \
    while (dx <= dy) {                                          \
      for (i = dy; i >= dx; i--) {                              \
	if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&          \
	    (((yorg - dx) >= 0) && ((yorg - dx) < h))) {        \
	  *(buf + (yorg - dx) * pitch + (xorg + i)) = val;      \
	}                                                       \
	if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&	\
	    (((yorg - i) >= 0) && ((yorg - i) < h))) {          \
	  *(buf + (yorg - i) * pitch + (xorg + dx)) = val;      \
	}                                                       \
	if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&	\
	    (((yorg - i) >= 0) && ((yorg - i) < h))) {          \
	  *(buf + (yorg - i) * pitch + (xorg - dx)) = val;      \
	}                                                       \
	if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&          \
	    (((yorg - dx) >= 0) && ((yorg - dx) < h))) {        \
	  *(buf + (yorg - dx) * pitch + (xorg - i)) = val;      \
	}                                                       \
	if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&          \
	    (((yorg + dx) >= 0) && ((yorg + dx) < h))) {        \
	  *(buf + (yorg + dx) * pitch + (xorg - i)) = val;      \
	}                                                       \
	if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&	\
	    (((yorg + i) >= 0) && ((yorg + i) < h))) {          \
	  *(buf + (yorg + i) * pitch + (xorg - dx)) = val;      \
	}                                                       \
	if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&	\
	    (((yorg + i) >= 0) && ((yorg + i) < h))) {          \
	  *(buf + (yorg + i) * pitch + (xorg + dx)) = val;      \
	}                                                       \
	if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&          \
	    (((yorg + dx) >= 0) && ((yorg + dx) < h))) {        \
	  *(buf + (yorg + dx) * pitch + (xorg + i)) = val;      \
	}                                                       \
      }                                                         \
      dx++;                                                     \
      n += inv_r;                                               \
      dy = r * sin(acos(n));                                    \
    }                                                           \
  }

void bytem_draw_filled_circle(int xorg, int yorg, int radius, int val, bytemap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(uint8_t, xorg, yorg, radius, val, m);
}

void wordm_draw_filled_circle(int xorg, int yorg, int radius, int val, wordmap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(uint16_t, xorg, yorg, radius, val, m);
}

void dwordm_draw_filled_circle(int xorg, int yorg, int radius, uint32_t val, dwordmap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(uint32_t, xorg, yorg, radius, val, m);
}

void floatm_draw_filled_circle(int xorg, int yorg, int radius, float val, floatmap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(float, xorg, yorg, radius, val, m);
}

/* fixed point operation */
void bitmap_fast_draw_circle(int x, int y, int r, uint8_t val, bitmap_t *m)
{
  signed long n = 0;
  signed long inv_r;
  int dx, dy, w, h;
  assert(m);
  assert(r > 1);
  inv_r = (1 / (real_t)r) * 0x10000L;
  //printf("inv_r %d\n", inv_r);
  dx = 0, dy = r - 1;
  w = bitmap_get_width(m);
  h = bitmap_get_height(m);
  while (dx <= dy) {
    /* octant 0 */
    if ((((x + dy) >= 0) && ((x + dy) < w)) &&
	(((y - dx) >= 0) && ((y - dx) < h))) {
      if (val != 0) bitmap_set_value(x + dy, y - dx, m);
      else bitmap_reset_value(x + dy, y - dx, m);
      //*(p->buffer+(y - dx)*p->header.pitch+(x + dy)) = col;
    }
    /* octant 1 */
    if ((((x + dx) >= 0) && ((x + dx) < w)) &&
	(((y - dy) >= 0) && ((y - dy) < h))) {
      if (val != 0) bitmap_set_value(x + dx, y - dy, m);
      else bitmap_reset_value(x + dx, y - dy, m);
      //*(p->buffer+(y - dy)*p->header.pitch+(x + dx)) = col;
    }
    /* octant 2 */
    if ((((x - dx) >= 0) && ((x - dx) < w)) &&
	(((y - dy) >= 0) && ((y - dy) < h))) {
      if (val != 0) bitmap_set_value(x - dx, y - dy, m);
      else bitmap_reset_value(x - dx, y - dy, m);
      //*(m->buffer+(y - dy)*m->header.pitch+(x - dx)) = col;
    }
    /* octant 3 */
    if ((((x - dy) >= 0) && ((x - dy) < w)) &&
	(((y - dx) >= 0) && ((y - dx) < h))) {
      if (val != 0) bitmap_set_value(x - dy, y - dx, m);
      else bitmap_reset_value(x - dy, y - dx, m);
      //*(m->buffer+(y - dx)*m->header.pitch+(x - dy)) = col;
    }
    /* octant 4 */
    if ((((x - dy) >= 0) && ((x - dy) < w)) &&
	(((y + dx) >= 0) && ((y + dx) < h))) {
      if (val != 0) bitmap_set_value(x - dy, y + dx, m);
      else bitmap_reset_value(x - dy, y + dx, m);
      //*(m->buffer+(y + dx)*m->header.pitch+(x - dy)) = col;
    }
    /* octant 5 */
    if ((((x - dx) >= 0) && ((x - dx) < w)) &&
	(((y + dy) >= 0) && ((y + dy) < h))) {
      if (val != 0) bitmap_set_value(x - dx, y + dy, m);
      else bitmap_reset_value(x - dx, y + dy, m);
      //*(m->buffer+(y + dy)*m->header.pitch+(x - dx)) = col;
    }
    /* octant 6 */
    if ((((x + dx) >= 0) && ((x + dx) < w)) &&
	(((y + dy) >= 0) && ((y + dy) < h))) {
      if (val != 0) bitmap_set_value(x + dx, y + dy, m);
      else bitmap_reset_value(x + dx, y + dy, m);
      //*(m->buffer+(y + dy)*m->header.pitch+(x + dx)) = col;
    }
    /* octant 7 */
    if ((((x + dy) >= 0) && ((x + dy) < w)) &&
	(((y + dx) >= 0) && ((y + dx) < h))) {
      if (val != 0) bitmap_set_value(x + dy, y + dx, m);
      else bitmap_reset_value(x + dy, y + dx, m);
      //*(m->buffer+(y + dx)*m->header.pitch+(x + dy)) = col;
    }
    dx++;
    n += inv_r;
    dy = (int)((r * SIN_OF_ACOS[(int)(n >> 6)]) >> 16);
    //printf("dy = %d,  SIN_OF_ACOS[(int)(n>>6)] = %d, n>>6 = %d\n", dy, SIN_OF_ACOS[(int)(n>>6)], (int)(n>>6));
  }
}

#define PIXM_FAST_DRAW_CIRCLE(type, x, y, r, v, m) {            \
    signed long n = 0;                                          \
    signed long inv_r;                                          \
    int dx, dy, w, h, pitch;                                    \
    type *buf;                                                  \
    assert(m);                                                  \
    assert(r > 1);                                              \
    inv_r = (1 / (real_t)(r)) * 0x10000L;                       \
    dx = 0, dy = r - 1;                                         \
    w = (m)->header.width;                                      \
    h = (m)->header.height;                                     \
    pitch = (m)->header.pitch / sizeof(*buf);                   \
    buf = (m)->buffer;                                          \
    while (dx <= dy) {                                          \
      if ((((x + dy) >= 0) && ((x + dy) < w)) &&                \
	  (((y - dx) >= 0) && ((y - dx) < h))) {                \
	*(buf + (y - dx) * pitch + (x + dy)) = v;               \
      }                                                         \
      if ((((x + dx) >= 0) && ((x + dx) < w)) &&                \
	  (((y - dy) >= 0) && ((y - dy) < h))) {                \
	*(buf + (y - dy) * pitch + (x + dx)) = v;               \
      }                                                         \
      if ((((x - dx) >= 0) && ((x - dx) < w)) &&                \
	  (((y - dy) >= 0) && ((y - dy) < h))) {                \
	*(buf + (y - dy) * pitch + (x - dx)) = v;               \
      }                                                         \
      if ((((x - dy) >= 0) && ((x - dy) < w)) &&                \
	  (((y - dx) >= 0) && ((y - dx) < h))) {                \
	*(buf + (y - dx) * pitch + (x - dy)) = v;               \
      }                                                         \
      if ((((x - dy) >= 0) && ((x - dy) < w)) &&                \
	  (((y + dx) >= 0) && ((y + dx) < h))) {                \
	*(buf + (y + dx) * pitch + (x - dy)) = v;               \
      }                                                         \
      if ((((x - dx) >= 0) && ((x - dx) < w)) &&                \
	  (((y + dy) >= 0) && ((y + dy) < h))) {                \
	*(buf + (y + dy) * pitch + (x - dx)) = v;               \
      }                                                         \
      if ((((x + dx) >= 0) && ((x + dx) < w)) &&                \
	  (((y + dy) >= 0) && ((y + dy) < h))) {                \
	*(buf + (y + dy) * pitch + (x + dx)) = v;               \
      }                                                         \
      if ((((x + dy) >= 0) && ((x + dy) < w)) &&                \
	  (((y + dx) >= 0) && ((y + dx) < h))) {                \
	*(buf + (y + dx) * pitch + (x + dy)) = v;               \
      }                                                         \
      dx++;                                                     \
      n += inv_r;                                               \
      dy = (int)(((r) * SIN_OF_ACOS[(int)(n>>6)]) >> 16);       \
    }                                                           \
  }

void bytem_fast_draw_circle(int xorg, int yorg, int radius, uint8_t val, bytemap_t *m)
{
  PIXM_FAST_DRAW_CIRCLE(uint8_t, xorg, yorg, radius, val, m);
}

void wordm_fast_draw_circle(int xorg, int yorg, int radius, uint16_t val, wordmap_t *m)
{
  PIXM_FAST_DRAW_CIRCLE(uint16_t, xorg, yorg, radius, val, m);
}

void dwordm_fast_draw_circle(int xorg, int yorg, int radius, uint32_t val, dwordmap_t *m)
{
  PIXM_FAST_DRAW_CIRCLE(uint32_t, xorg, yorg, radius, val, m);
}

void floatm_fast_draw_circle(int xorg, int yorg, int radius, float val, floatmap_t *m)
{
  PIXM_FAST_DRAW_CIRCLE(float, xorg, yorg, radius, val, m);
}

void bitmap_fast_draw_filled_circle(int xorg, int yorg, int radius, uint8_t val, bitmap_t *m)
{
  signed long n = 0;
  signed long inv_r;
  int dx, dy, i, w, h;
  assert(m);
  assert(radius > 1.0);
  inv_r = (1.0 / (real_t)radius) * 0x10000L;
  dx = 0;
  dy = radius - 1;
  w = bitmap_get_width(m);
  h = bitmap_get_height(m);
  while (dx <= dy) {
    for (i = dy; i >= dx; i--) {
      /* octant 0 */
      if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&
	  (((yorg - dx) >= 0) && ((yorg - dx) < h))) {
	if (val != 0) bitmap_set_value(xorg + i, yorg - dx, m);
	else bitmap_reset_value(xorg + i, yorg - dx, m);
	//*(m->buffer+(yorg - dx)*m->header.pitch+(xorg + i)) = val;
      }
      /* octant 1 */
      if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&
	  (((yorg - i) >= 0) && ((yorg - i) < h))) {
	if (val != 0) bitmap_set_value(xorg + dx, yorg - i, m);
	else bitmap_reset_value(xorg + dx, yorg - i, m);
	//*(m->buffer+(yorg - i)*m->header.pitch+(xorg + dx)) = val;
      }
      /* octant 2 */
      if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&
	  (((yorg - i) >= 0) && ((yorg - i) < h))) {
	if (val != 0) bitmap_set_value(xorg - dx, yorg - i, m);
	else bitmap_reset_value(xorg - dx, yorg - i, m);
	//*(m->buffer+(yorg - i)*m->header.pitch+(xorg - dx)) = val;
      }
      /* octant 3 */
      if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&
	  (((yorg - dx) >= 0) && ((yorg - dx) < h))) {
	if (val != 0) bitmap_set_value(xorg - i, yorg - dx, m);
	else bitmap_reset_value(xorg - i, yorg - dx, m);
	//*(m->buffer+(yorg - dx)*m->header.pitch+(xorg - i)) = val;
      }
      /* octant 4 */
      if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&
	  (((yorg + dx) >= 0) && ((yorg + dx) < h))) {
	if (val != 0) bitmap_set_value(xorg - i, yorg + dx, m);
	else bitmap_reset_value(xorg - i, yorg + dx, m);
	//*(m->buffer+(yorg + dx)*m->header.pitch+(xorg - i)) = val;
      }
      /* octant 5 */
      if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&
	  (((yorg + i) >= 0) && ((yorg + i) < h))) {
	if (val != 0) bitmap_set_value(xorg - dx, yorg + i, m);
	else bitmap_reset_value(xorg - dx, yorg + i, m);
	//*(m->buffer+(yorg + i)*m->header.pitch+(xorg - dx)) = val;
      }
      /* octant 6 */
      if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&
	  (((yorg + i) >= 0) && ((yorg + i) < h))) {
	if (val != 0) bitmap_set_value(xorg + dx, yorg + i, m);
	else bitmap_reset_value(xorg + dx, yorg + i, m);
	//*(m->buffer+(yorg + i)*m->header.pitch+(xorg + dx)) = val;
      }
      /* octant 7 */
      if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&
	  (((yorg + dx) >= 0) && ((yorg + dx) < h))) {
	if (val != 0) bitmap_set_value(xorg + i, yorg + dx, m);
	else bitmap_reset_value(xorg + i, yorg + dx, m);
	//*(m->buffer+(yorg + dx)*m->header.pitch+(xorg + i)) = val;
      }
    }
    dx++;
    n += inv_r;
    dy = (int)((radius * SIN_OF_ACOS[(int)(n >> 6)]) >> 16);
  }
}

#define PIXM_FAST_DRAW_FILLED_CIRCLE(type, xorg, yorg, r, val, m) {     \
    signed long n = 0;							\
    signed long inv_r;							\
    int dx, dy, i, w, h, pitch;                                         \
    type *buf;								\
    assert(m);                                                          \
    assert(r > 1.0);							\
    inv_r = (1 / (real_t)(r)) * 0x10000L;                               \
    dx = 0;								\
    dy = r - 1;								\
    w = (m)->header.width;                                              \
    h = (m)->header.height;                                             \
    pitch = (m)->header.pitch / sizeof(*buf);                           \
    buf = (m)->buffer;                                                  \
    while (dx <= dy) {							\
      for (i = dy; i >= dx; i--) {					\
	if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&                  \
	    (((yorg - dx) >= 0) && ((yorg - dx) < h))) {                \
	  *(buf + (yorg - dx) * pitch + (xorg + i)) = val;              \
	}								\
	if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&                \
	    (((yorg - i) >= 0) && ((yorg - i) < h))) {                  \
	  *(buf + (yorg - i) * pitch + (xorg + dx)) = val;              \
	}								\
	if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&                \
	    (((yorg - i) >= 0) && ((yorg - i) < h))) {                  \
	  *(buf + (yorg - i) * pitch + (xorg - dx)) = val;              \
	}								\
	if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&                  \
	    (((yorg - dx) >= 0) && ((yorg - dx) < h))) {                \
	  *(buf + (yorg - dx) * pitch + (xorg - i)) = val;              \
	}								\
	if ((((xorg - i) >= 0) && ((xorg - i) < w)) &&                  \
	    (((yorg + dx) >= 0) && ((yorg + dx) < h))) {                \
	  *(buf + (yorg + dx) * pitch + (xorg - i)) = val;              \
	}								\
	if ((((xorg - dx) >= 0) && ((xorg - dx) < w)) &&                \
	    (((yorg + i) >= 0) && ((yorg + i) < h))) {                  \
	  *(buf + (yorg + i) * pitch + (xorg - dx)) = val;              \
	}								\
	if ((((xorg + dx) >= 0) && ((xorg + dx) < w)) &&                \
	    (((yorg + i) >= 0) && ((yorg + i) < h))) {                  \
	  *(buf + (yorg + i) * pitch + (xorg + dx)) = val;              \
	}								\
	if ((((xorg + i) >= 0) && ((xorg + i) < w)) &&                  \
	    (((yorg + dx) >= 0) && ((yorg + dx) < h))) {                \
	  *(buf + (yorg + dx) * pitch + (xorg + i)) = val;              \
	}								\
      }									\
      dx++;								\
      n += inv_r;							\
      dy = (int)(((r) * SIN_OF_ACOS[(int)(n >> 6)]) >> 16);             \
    }									\
  }

void bytem_fast_draw_filled_circle(int xorg, int yorg, int radius, int val, bytemap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(uint8_t, xorg, yorg, radius, val, m);
}

void wordm_fast_draw_filled_circle(int xorg, int yorg, int radius, int val, wordmap_t *m)
{
  PIXM_FAST_DRAW_FILLED_CIRCLE(uint16_t, xorg, yorg, radius, val, m);
}

void dwordm_fast_draw_filled_circle(int xorg, int yorg, int radius, uint32_t val, dwordmap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(uint32_t, xorg, yorg, radius, val, m);
}

void floatm_fast_draw_filled_circle(int xorg, int yorg, int radius, float val, floatmap_t *m)
{
  PIXM_DRAW_FILLED_CIRCLE(float, xorg, yorg, radius, val, m);
}