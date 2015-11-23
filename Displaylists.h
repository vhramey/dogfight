#ifndef DISPLAYLISTS_H
#define DISPLAYLISTS_H

#include <GLUT/glut.h>
#include <math.h>

//GLuint plane;

struct sample_MATERIAL{
 GLfloat ambient[3];
 GLfloat diffuse[3];
 GLfloat specular[3];
 GLfloat emission[3];
 GLfloat alpha;
 GLfloat phExp;
 int   texture;
};

static sample_MATERIAL materials [1] = {
	{{0.117647f,0.117647f,0.117647f},	{0.752941f,0.752941f,0.752941f},	{0.752941f,0.752941f,0.752941f},	{0.0f,0.0f,0.0f},	1.0f,8.0f,-1} //Explorer Default
};

// 237 Verticies
// 0 Texture Coordinates
// 366 Normals
// 410 Triangles

static short face_indicies[410][9] = {
// Cone
	{4,1,0 ,0,1,2 ,0,0,0}, {3,4,0 ,3,0,2 ,0,0,0}, {5,2,1 ,4,5,1 ,0,0,0},
	{4,5,1 ,0,4,1 ,0,0,0}, {7,4,3 ,6,7,8 ,0,0,0}, {6,7,3 ,9,6,8 ,0,0,0},
	{8,5,4 ,10,11,7 ,0,0,0}, {7,8,4 ,6,10,7 ,0,0,0}, {10,7,6 ,12,13,14 ,0,0,0},
	{9,10,6 ,14,12,14 ,0,0,0}, {11,8,7 ,15,16,13 ,0,0,0}, {10,11,7 ,12,15,13 ,0,0,0},
	{13,10,9 ,17,18,19 ,0,0,0}, {12,13,9 ,19,17,19 ,0,0,0}, {14,11,10 ,20,20,18 ,0,0,0},
	{13,14,10 ,17,20,18 ,0,0,0}, {16,13,12 ,21,22,23 ,0,0,0}, {15,16,12 ,23,21,23 ,0,0,0},
	{17,14,13 ,24,25,22 ,0,0,0}, {16,17,13 ,21,24,22 ,0,0,0}, {19,16,15 ,26,27,28 ,0,0,0},
	{18,19,15 ,29,26,28 ,0,0,0}, {20,17,16 ,30,31,27 ,0,0,0}, {19,20,16 ,26,30,27 ,0,0,0},
	{1,19,18 ,32,33,34 ,0,0,0}, {0,1,18 ,35,32,34 ,0,0,0}, {2,20,19 ,36,37,33 ,0,0,0},
	{1,2,19 ,32,36,33 ,0,0,0}
// Wings
	, {40,38,37 ,38,38,38 ,0,0,0}, {39,40,37 ,38,38,38 ,0,0,0}, {26,24,23 ,39,40,41 ,0,0,0},
	{25,26,23 ,42,39,41 ,0,0,0}, {44,42,41 ,43,43,43 ,0,0,0}, {43,44,41 ,43,43,43 ,0,0,0},
	{22,28,27 ,44,45,46 ,0,0,0}, {21,22,27 ,47,44,46 ,0,0,0}, {26,28,22 ,39,48,48 ,0,0,0},
	{24,26,22 ,40,39,48 ,0,0,0}, {46,45,48 ,49,50,51 ,0,0,0}, {47,46,48 ,52,49,51 ,0,0,0},
	{30,26,25 ,53,39,42 ,0,0,0}, {29,30,25 ,54,53,42 ,0,0,0}, {32,28,26 ,55,48,39 ,0,0,0},
	{30,32,26 ,53,55,39 ,0,0,0}, {31,27,28 ,56,46,45 ,0,0,0}, {32,31,28 ,57,56,45 ,0,0,0},
	{49,47,48 ,58,52,51 ,0,0,0}, {50,49,48 ,59,58,51 ,0,0,0}, {34,22,21 ,60,44,47 ,0,0,0},
	{33,34,21 ,61,60,47 ,0,0,0}, {36,24,22 ,62,40,48 ,0,0,0}, {34,36,22 ,63,62,48 ,0,0,0},
	{35,23,24 ,64,41,40 ,0,0,0}, {36,35,24 ,62,64,40 ,0,0,0}, {51,45,46 ,65,50,49 ,0,0,0},
	{52,51,46 ,66,65,49 ,0,0,0}, {38,34,33 ,67,68,67 ,0,0,0}, {37,38,33 ,69,67,67 ,0,0,0},
	{40,36,34 ,70,62,63 ,0,0,0}, {38,40,34 ,71,70,63 ,0,0,0}, {39,35,36 ,72,73,72 ,0,0,0},
	{40,39,36 ,74,72,72 ,0,0,0}, {37,33,35 ,75,76,75 ,0,0,0}, {39,37,35 ,77,75,75 ,0,0,0},
	{42,30,29 ,78,79,54 ,0,0,0}, {41,42,29 ,80,78,54 ,0,0,0}, {44,32,30 ,81,55,53 ,0,0,0},
	{42,44,30 ,81,81,53 ,0,0,0}, {43,31,32 ,82,83,82 ,0,0,0}, {44,43,32 ,84,82,82 ,0,0,0},
	{41,29,31 ,85,86,85 ,0,0,0}, {43,41,31 ,87,85,85 ,0,0,0}, {45,21,27 ,88,89,90 ,0,0,0},
	{48,45,27 ,91,88,90 ,0,0,0}, {47,25,23 ,92,93,94 ,0,0,0}, {46,47,23 ,95,92,94 ,0,0,0},
	{49,29,25 ,96,96,96 ,0,0,0}, {47,49,25 ,92,97,93 ,0,0,0}, {50,31,29 ,38,38,38 ,0,0,0},
	{49,50,29 ,38,38,38 ,0,0,0}, {48,27,31 ,91,90,98 ,0,0,0}, {50,48,31 ,99,99,99 ,0,0,0},
	{51,33,21 ,100,101,89 ,0,0,0}, {45,51,21 ,88,100,89 ,0,0,0},
	{52,35,33 ,38,38,38 ,0,0,0}, {51,52,33 ,38,38,38 ,0,0,0}, {46,23,35 ,95,94,102 ,0,0,0},
	{52,46,35 ,103,95,102 ,0,0,0}
// Tail_Fin
	, {72,70,69 ,38,38,38 ,0,0,0}, {71,72,69 ,38,38,38 ,0,0,0}, {58,56,55 ,104,105,106 ,0,0,0},
	{57,58,55 ,107,104,106 ,0,0,0}, {76,74,73 ,43,43,43 ,0,0,0},
	{75,76,73 ,43,43,43 ,0,0,0}, {54,60,59 ,108,109,110 ,0,0,0},
	{53,54,59 ,111,108,110 ,0,0,0}, {58,60,54 ,104,109,108 ,0,0,0},
	{56,58,54 ,105,104,108 ,0,0,0}, {78,77,80 ,112,113,114 ,0,0,0},
	{79,78,80 ,115,112,114 ,0,0,0}, {62,58,57 ,116,104,107 ,0,0,0},
	{61,62,57 ,117,116,107 ,0,0,0}, {64,60,58 ,118,109,104 ,0,0,0},
	{62,64,58 ,116,118,104 ,0,0,0}, {63,59,60 ,119,110,109 ,0,0,0},
	{64,63,60 ,118,119,109 ,0,0,0}, {81,79,80 ,120,115,114 ,0,0,0},
	{82,81,80 ,121,120,114 ,0,0,0}, {66,54,53 ,122,108,111 ,0,0,0},
	{65,66,53 ,123,122,111 ,0,0,0}, {68,56,54 ,124,105,108 ,0,0,0},
	{66,68,54 ,122,124,108 ,0,0,0}, {67,55,56 ,125,106,105 ,0,0,0},
	{68,67,56 ,124,125,105 ,0,0,0}, {83,77,78 ,126,113,112 ,0,0,0},
	{84,83,78 ,127,126,112 ,0,0,0}, {70,66,65 ,128,129,128 ,0,0,0},
	{69,70,65 ,130,128,128 ,0,0,0}, {72,68,66 ,131,124,122 ,0,0,0},
	{70,72,66 ,132,131,122 ,0,0,0}, {71,67,68 ,133,134,133 ,0,0,0},
	{72,71,68 ,135,133,133 ,0,0,0}, {69,65,67 ,136,137,136 ,0,0,0},
	{71,69,67 ,138,136,136 ,0,0,0}, {74,62,61 ,139,140,141 ,0,0,0},
	{73,74,61 ,142,139,141 ,0,0,0}, {76,64,62 ,143,118,116 ,0,0,0},
	{74,76,62 ,139,143,116 ,0,0,0}, {75,63,64 ,144,145,144 ,0,0,0},
	{76,75,64 ,146,144,144 ,0,0,0}, {73,61,63 ,147,148,147 ,0,0,0},
	{75,73,63 ,149,147,147 ,0,0,0}, {77,53,59 ,150,151,152 ,0,0,0},
	{80,77,59 ,153,150,152 ,0,0,0}, {79,57,55 ,154,155,156 ,0,0,0},
	{78,79,55 ,157,154,156 ,0,0,0}, {81,61,57 ,120,148,158 ,0,0,0},
	{79,81,57 ,154,159,155 ,0,0,0}, {82,63,61 ,38,38,38 ,0,0,0},
	{81,82,61 ,38,38,38 ,0,0,0}, {80,59,63 ,153,152,160 ,0,0,0},
	{82,80,63 ,161,161,161 ,0,0,0}, {83,65,53 ,162,163,151 ,0,0,0},
	{77,83,53 ,150,162,151 ,0,0,0}, {84,67,65 ,38,38,38 ,0,0,0},
	{83,84,65 ,38,38,38 ,0,0,0}, {78,55,67 ,157,156,164 ,0,0,0},
	{84,78,67 ,165,157,164 ,0,0,0}
// Body
	, {104,105,86 ,166,167,168 ,0,0,0}, {85,104,86 ,169,166,168 ,0,0,0},
	{105,106,87 ,167,170,171 ,0,0,0}, {86,105,87 ,168,167,171 ,0,0,0},
	{106,107,88 ,170,172,173 ,0,0,0}, {87,106,88 ,171,170,173 ,0,0,0},
	{107,108,89 ,174,175,176 ,0,0,0}, {88,107,89 ,177,174,176 ,0,0,0},
	{108,109,90 ,175,178,179 ,0,0,0}, {89,108,90 ,176,175,179 ,0,0,0},
	{109,110,91 ,180,181,182 ,0,0,0}, {90,109,91 ,183,180,182 ,0,0,0},
	{110,111,92 ,181,184,185 ,0,0,0}, {91,110,92 ,182,181,185 ,0,0,0},
	{111,112,93 ,184,186,187 ,0,0,0}, {92,111,93 ,185,184,187 ,0,0,0},
	{112,113,94 ,186,188,189 ,0,0,0}, {93,112,94 ,187,186,189 ,0,0,0},
	{113,114,95 ,190,191,192 ,0,0,0}, {94,113,95 ,193,193,193 ,0,0,0},
	{114,115,96 ,191,194,195 ,0,0,0}, {95,114,96 ,192,191,195 ,0,0,0},
	{115,116,97 ,194,196,197 ,0,0,0}, {96,115,97 ,195,194,197 ,0,0,0},
	{116,117,98 ,198,199,200 ,0,0,0}, {97,116,98 ,201,198,200 ,0,0,0},
	{117,118,99 ,202,203,204 ,0,0,0}, {98,117,99 ,205,202,204 ,0,0,0},
	{118,119,100 ,206,207,208 ,0,0,0}, {99,118,100 ,209,206,208 ,0,0,0},
	{119,120,101 ,207,210,211 ,0,0,0}, {100,119,101 ,208,207,211 ,0,0,0},
	{120,121,102 ,210,212,213 ,0,0,0}, {101,120,102 ,211,210,213 ,0,0,0},
	{121,122,103 ,212,214,215 ,0,0,0}, {102,121,103 ,213,212,215 ,0,0,0},
	{122,104,85 ,214,216,217 ,0,0,0}, {103,122,85 ,215,214,217 ,0,0,0},
	{123,124,105 ,218,219,167 ,0,0,0}, {104,123,105 ,166,218,167 ,0,0,0},
	{124,125,106 ,219,220,170 ,0,0,0}, {105,124,106 ,167,219,170 ,0,0,0},
	{125,126,107 ,220,221,172 ,0,0,0}, {106,125,107 ,170,220,172 ,0,0,0},
	{126,127,108 ,221,222,175 ,0,0,0}, {107,126,108 ,223,223,223 ,0,0,0},
	{127,128,109 ,222,224,225 ,0,0,0}, {108,127,109 ,175,222,178 ,0,0,0},
	{128,129,110 ,224,226,181 ,0,0,0}, {109,128,110 ,225,224,181 ,0,0,0},
	{129,130,111 ,226,227,184 ,0,0,0}, {110,129,111 ,181,226,184 ,0,0,0},
	{130,131,112 ,227,228,186 ,0,0,0}, {111,130,112 ,184,227,186 ,0,0,0},
	{131,132,113 ,228,229,188 ,0,0,0}, {112,131,113 ,186,228,188 ,0,0,0},
	{132,133,114 ,229,230,191 ,0,0,0}, {113,132,114 ,190,229,191 ,0,0,0},
	{133,134,115 ,230,231,194 ,0,0,0}, {114,133,115 ,191,230,194 ,0,0,0},
	{134,135,116 ,231,232,196 ,0,0,0}, {115,134,116 ,194,231,196 ,0,0,0},
	{135,136,117 ,232,233,234 ,0,0,0}, {116,135,117 ,196,232,234 ,0,0,0},
	{136,137,118 ,235,236,206 ,0,0,0}, {117,136,118 ,237,235,206 ,0,0,0},
	{137,138,119 ,236,238,207 ,0,0,0}, {118,137,119 ,206,236,207 ,0,0,0},
	{138,139,120 ,238,239,210 ,0,0,0}, {119,138,120 ,207,238,210 ,0,0,0},
	{139,140,121 ,239,240,212 ,0,0,0}, {120,139,121 ,210,239,212 ,0,0,0},
	{140,141,122 ,240,241,214 ,0,0,0}, {121,140,122 ,212,240,214 ,0,0,0},
	{141,123,104 ,241,242,216 ,0,0,0}, {122,141,104 ,214,241,216 ,0,0,0},
	{142,143,124 ,243,244,219 ,0,0,0}, {123,142,124 ,218,243,219 ,0,0,0},
	{143,144,125 ,244,245,220 ,0,0,0}, {124,143,125 ,219,244,220 ,0,0,0},
	{144,145,126 ,245,246,221 ,0,0,0}, {125,144,126 ,220,245,221 ,0,0,0},
	{145,146,127 ,246,247,222 ,0,0,0}, {126,145,127 ,221,246,222 ,0,0,0},
	{146,147,128 ,247,248,224 ,0,0,0}, {127,146,128 ,222,247,224 ,0,0,0},
	{147,148,129 ,248,249,226 ,0,0,0}, {128,147,129 ,224,248,226 ,0,0,0},
	{148,149,130 ,249,250,227 ,0,0,0}, {129,148,130 ,226,249,227 ,0,0,0},
	{149,150,131 ,250,251,228 ,0,0,0}, {130,149,131 ,227,250,228 ,0,0,0},
	{150,151,132 ,252,252,252 ,0,0,0}, {131,150,132 ,228,253,229 ,0,0,0},
	{151,152,133 ,254,255,256 ,0,0,0}, {132,151,133 ,257,254,256 ,0,0,0},
	{152,153,134 ,255,258,259 ,0,0,0}, {133,152,134 ,256,255,259 ,0,0,0},
	{153,154,135 ,258,260,261 ,0,0,0}, {134,153,135 ,259,258,261 ,0,0,0},
	{154,155,136 ,260,262,263 ,0,0,0}, {135,154,136 ,261,260,263 ,0,0,0},
	{155,156,137 ,264,265,236 ,0,0,0}, {136,155,137 ,235,264,236 ,0,0,0},
	{156,157,138 ,266,267,238 ,0,0,0}, {137,156,138 ,236,265,238 ,0,0,0},
	{157,158,139 ,267,268,239 ,0,0,0}, {138,157,139 ,238,267,239 ,0,0,0},
	{158,159,140 ,268,269,240 ,0,0,0}, {139,158,140 ,239,268,240 ,0,0,0},
	{159,160,141 ,269,270,241 ,0,0,0}, {140,159,141 ,240,269,241 ,0,0,0},
	{160,142,123 ,271,271,242 ,0,0,0}, {141,160,123 ,241,270,272 ,0,0,0},
	{162,161,85 ,273,274,169 ,0,0,0}, {86,162,85 ,168,273,169 ,0,0,0},
	{163,162,86 ,275,273,168 ,0,0,0}, {87,163,86 ,171,275,168 ,0,0,0},
	{164,163,87 ,276,275,171 ,0,0,0}, {88,164,87 ,173,276,171 ,0,0,0},
	{165,164,88 ,277,278,177 ,0,0,0}, {89,165,88 ,176,277,177 ,0,0,0},
	{166,165,89 ,279,277,176 ,0,0,0}, {90,166,89 ,179,279,176 ,0,0,0},
	{167,166,90 ,280,281,183 ,0,0,0}, {91,167,90 ,182,280,183 ,0,0,0},
	{168,167,91 ,282,280,182 ,0,0,0}, {92,168,91 ,283,282,182 ,0,0,0},
	{169,168,92 ,284,282,283 ,0,0,0}, {93,169,92 ,285,284,283 ,0,0,0},
	{170,169,93 ,286,284,285 ,0,0,0}, {94,170,93 ,287,286,285 ,0,0,0},
	{171,170,94 ,288,289,288 ,0,0,0}, {95,171,94 ,290,288,288 ,0,0,0},
	{172,171,95 ,291,292,290 ,0,0,0}, {96,172,95 ,293,291,290 ,0,0,0},
	{173,172,96 ,294,291,293 ,0,0,0}, {97,173,96 ,295,294,293 ,0,0,0},
	{174,173,97 ,296,294,295 ,0,0,0}, {98,174,97 ,200,296,295 ,0,0,0},
	{175,174,98 ,297,298,205 ,0,0,0}, {99,175,98 ,204,297,205 ,0,0,0},
	{176,175,99 ,299,300,209 ,0,0,0}, {100,176,99 ,208,299,209 ,0,0,0},
	{177,176,100 ,301,299,208 ,0,0,0}, {101,177,100 ,302,301,208 ,0,0,0},
	{178,177,101 ,303,301,302 ,0,0,0}, {102,178,101 ,304,303,302 ,0,0,0},
	{179,178,102 ,305,303,304 ,0,0,0}, {103,179,102 ,306,305,304 ,0,0,0},
	{161,179,103 ,307,305,306 ,0,0,0}, {85,161,103 ,308,307,306 ,0,0,0},
	{181,180,161 ,309,310,274 ,0,0,0}, {162,181,161 ,273,309,274 ,0,0,0},
	{182,181,162 ,311,309,273 ,0,0,0}, {163,182,162 ,275,311,273 ,0,0,0},
	{183,182,163 ,312,311,275 ,0,0,0}, {164,183,163 ,276,312,275 ,0,0,0},
	{184,183,164 ,313,312,276 ,0,0,0}, {165,184,164 ,314,313,276 ,0,0,0},
	{185,184,165 ,315,313,314 ,0,0,0}, {166,185,165 ,316,315,314 ,0,0,0},
	{186,185,166 ,317,315,316 ,0,0,0}, {167,186,166 ,280,317,316 ,0,0,0},
	{187,186,167 ,318,317,280 ,0,0,0}, {168,187,167 ,282,318,280 ,0,0,0},
	{188,187,168 ,319,318,282 ,0,0,0}, {169,188,168 ,284,319,282 ,0,0,0},
	{189,188,169 ,320,319,284 ,0,0,0}, {170,189,169 ,321,320,284 ,0,0,0},
	{190,189,170 ,322,320,321 ,0,0,0}, {171,190,170 ,292,322,321 ,0,0,0},
	{191,190,171 ,323,322,292 ,0,0,0}, {172,191,171 ,291,323,292 ,0,0,0},
	{192,191,172 ,324,323,291 ,0,0,0}, {173,192,172 ,294,324,291 ,0,0,0},
	{193,192,173 ,325,324,294 ,0,0,0}, {174,193,173 ,326,325,294 ,0,0,0},
	{194,193,174 ,327,327,327 ,0,0,0}, {175,194,174 ,328,329,326 ,0,0,0},
	{195,194,175 ,330,329,328 ,0,0,0}, {176,195,175 ,299,330,328 ,0,0,0},
	{196,195,176 ,331,330,299 ,0,0,0}, {177,196,176 ,301,331,299 ,0,0,0},
	{197,196,177 ,332,331,301 ,0,0,0}, {178,197,177 ,303,332,301 ,0,0,0},
	{198,197,178 ,333,332,303 ,0,0,0}, {179,198,178 ,305,333,303 ,0,0,0},
	{180,198,179 ,334,333,305 ,0,0,0}, {161,180,179 ,307,334,305 ,0,0,0},
	{200,199,180 ,335,335,310 ,0,0,0}, {181,200,180 ,309,336,337 ,0,0,0},
	{201,200,181 ,338,336,309 ,0,0,0}, {182,201,181 ,339,338,309 ,0,0,0},
	{202,201,182 ,340,338,339 ,0,0,0}, {183,202,182 ,312,340,339 ,0,0,0},
	{203,202,183 ,341,340,312 ,0,0,0}, {184,203,183 ,313,341,312 ,0,0,0},
	{204,203,184 ,342,341,313 ,0,0,0}, {185,204,184 ,315,342,313 ,0,0,0},
	{205,204,185 ,343,342,315 ,0,0,0}, {186,205,185 ,317,343,315 ,0,0,0},
	{206,205,186 ,344,343,317 ,0,0,0}, {187,206,186 ,318,344,317 ,0,0,0},
	{207,206,187 ,345,344,318 ,0,0,0}, {188,207,187 ,319,345,318 ,0,0,0},
	{208,207,188 ,346,346,346 ,0,0,0}, {189,208,188 ,320,347,319 ,0,0,0},
	{209,208,189 ,348,349,320 ,0,0,0}, {190,209,189 ,350,348,320 ,0,0,0},
	{210,209,190 ,351,348,350 ,0,0,0}, {191,210,190 ,352,351,350 ,0,0,0},
	{211,210,191 ,353,351,352 ,0,0,0}, {192,211,191 ,354,353,352 ,0,0,0},
	{212,211,192 ,355,353,354 ,0,0,0}, {193,212,192 ,356,355,354 ,0,0,0},
	{213,212,193 ,357,358,358 ,0,0,0}, {194,213,193 ,329,359,359 ,0,0,0},
	{214,213,194 ,360,357,360 ,0,0,0}, {195,214,194 ,330,361,329 ,0,0,0},
	{215,214,195 ,362,361,330 ,0,0,0}, {196,215,195 ,331,362,330 ,0,0,0},
	{216,215,196 ,363,362,331 ,0,0,0}, {197,216,196 ,332,363,331 ,0,0,0},
	{217,216,197 ,364,363,332 ,0,0,0}, {198,217,197 ,333,364,332 ,0,0,0},
	{199,217,198 ,365,364,333 ,0,0,0}, {180,199,198 ,334,365,333 ,0,0,0}
// Cap
	, {154,156,155 ,38,38,38 ,0,0,0}, {153,156,154 ,38,38,38 ,0,0,0},
	{153,152,156 ,38,38,38 ,0,0,0}, {152,151,156 ,38,38,38 ,0,0,0},
	{150,149,151 ,38,38,38 ,0,0,0}, {151,157,156 ,38,38,38 ,0,0,0},
	{149,157,151 ,38,38,38 ,0,0,0}, {148,157,149 ,38,38,38 ,0,0,0},
	{148,158,157 ,38,38,38 ,0,0,0}, {147,158,148 ,38,38,38 ,0,0,0},
	{147,146,158 ,38,38,38 ,0,0,0}, {146,145,158 ,38,38,38 ,0,0,0},
	{145,159,158 ,38,38,38 ,0,0,0}, {144,159,145 ,38,38,38 ,0,0,0},
	{144,160,159 ,38,38,38 ,0,0,0}, {143,160,144 ,38,38,38 ,0,0,0},
	{143,142,160 ,38,38,38 ,0,0,0}, {231,232,230 ,43,43,43 ,0,0,0},
	{230,232,229 ,43,43,43 ,0,0,0}, {232,228,229 ,43,43,43 ,0,0,0},
	{232,227,228 ,43,43,43 ,0,0,0}, {227,225,226 ,43,43,43 ,0,0,0},
	{232,233,227 ,43,43,43 ,0,0,0}, {227,233,225 ,43,43,43 ,0,0,0},
	{225,233,224 ,43,43,43 ,0,0,0}, {233,234,224 ,43,43,43 ,0,0,0},
	{224,234,223 ,43,43,43 ,0,0,0}, {234,222,223 ,43,43,43 ,0,0,0},
	{234,221,222 ,43,43,43 ,0,0,0}, {234,235,221 ,43,43,43 ,0,0,0},
	{221,235,220 ,43,43,43 ,0,0,0}, {235,236,220 ,43,43,43 ,0,0,0},
	{220,236,219 ,43,43,43 ,0,0,0}, {236,218,219 ,43,43,43 ,0,0,0}
};
static GLfloat vertices [237][3] = {
{-0.463417f,1.68477e-18f,-0.0245975f},{-0.481708f,2.52339e-18f,-0.0383259f},{-0.5f,3.36201e-18f,-0.0520542f},
{-0.463417f,0.0214686f,-0.0349363f},{-0.481708f,0.0107354f,-0.0434957f},{-0.5f,2.14732e-06f,-0.0520552f},
{-0.463417f,0.026771f,-0.0581672f},{-0.481708f,0.0133868f,-0.0551124f},{-0.5f,2.67767e-06f,-0.0520576f},
{-0.463417f,0.0119142f,-0.076797f},{-0.481708f,0.00595769f,-0.0644282f},{-0.5f,1.19167e-06f,-0.0520594f},
{-0.463417f,-0.0119142f,-0.076797f},{-0.481708f,-0.00595769f,-0.0644282f},{-0.5f,-1.19167e-06f,-0.0520594f},
{-0.463417f,-0.026771f,-0.0581672f},{-0.481708f,-0.0133868f,-0.0551124f},{-0.5f,-2.67767e-06f,-0.0520576f},
{-0.463417f,-0.0214686f,-0.0349363f},{-0.481708f,-0.0107354f,-0.0434958f},{-0.5f,-2.14732e-06f,-0.0520552f},
{-0.208122f,-0.188836f,-0.0462171f},{-0.182364f,-0.188836f,-0.0254404f},{-0.0418554f,-0.188836f,-0.0478694f},
{-0.0539225f,-0.188836f,-0.0402825f},{-0.0418554f,0.188836f,-0.0478694f},{-0.0539225f,0.188836f,-0.0402825f},
{-0.208122f,0.188836f,-0.0462171f},{-0.182364f,0.188836f,-0.0254404f},{-0.0418554f,0.407481f,-0.0505029f},
{-0.0539225f,0.407481f,-0.0402825f},{-0.208122f,0.407481f,-0.0505202f},{-0.182364f,0.407481f,-0.0254404f},
{-0.208122f,-0.407481f,-0.0499684f},{-0.182364f,-0.407481f,-0.0254404f},{-0.0418554f,-0.407481f,-0.0501652f},
{-0.0539225f,-0.407481f,-0.0402825f},{-0.149638f,-0.438716f,-0.0394901f},{-0.142f,-0.438716f,-0.0333298f},
{-0.10034f,-0.438716f,-0.03998f},{-0.103918f,-0.438716f,-0.0377305f},{-0.0959844f,0.438716f,-0.0405675f},
{-0.100194f,0.438716f,-0.0379205f},{-0.153993f,0.438716f,-0.039991f},{-0.145006f,0.438716f,-0.0327422f},
{-0.20819f,-0.188836f,-0.0530816f},{-0.0419236f,-0.188836f,-0.0547339f},{-0.0419236f,0.188836f,-0.0547339f},
{-0.20819f,0.188836f,-0.0530816f},{-0.0419236f,0.407481f,-0.0504308f},{-0.20819f,0.407481f,-0.0504481f},
{-0.20819f,-0.407481f,-0.0507859f},{-0.0419236f,-0.407481f,-0.0509826f},{0.370794f,-0.0556121f,0.0563368f},
{0.385037f,-0.0556121f,0.0624556f},{0.462727f,-0.0556121f,0.0558502f},{0.456055f,-0.0556121f,0.0580846f},
{0.462727f,0.0556121f,0.0558502f},{0.456055f,0.0556121f,0.0580846f},{0.370794f,0.0556121f,0.0563368f},
{0.385037f,0.0556121f,0.0624556f},{0.462727f,0.120003f,0.0550746f},{0.456055f,0.120003f,0.0580846f},
{0.370794f,0.120003f,0.0550695f},{0.385037f,0.120003f,0.0624556f},{0.370794f,-0.120003f,0.0552321f},
{0.385037f,-0.120003f,0.0624556f},{0.462727f,-0.120003f,0.0551741f},{0.456055f,-0.120003f,0.0580846f},
{0.403132f,-0.129202f,0.0583179f},{0.407355f,-0.129202f,0.0601321f},{0.43039f,-0.129202f,0.0581736f},
{0.428411f,-0.129202f,0.0588361f},{0.432798f,0.129202f,0.0580006f},{0.43047f,0.129202f,0.0587802f},
{0.400723f,0.129202f,0.0581704f},{0.405693f,0.129202f,0.0603052f},{0.370757f,-0.0556121f,0.0543152f},
{0.462689f,-0.0556121f,0.0538286f},{0.462689f,0.0556121f,0.0538286f},{0.370757f,0.0556121f,0.0543152f},
{0.462689f,0.120003f,0.0550959f},{0.370757f,0.120003f,0.0550908f},{0.370757f,-0.120003f,0.0549913f},
{0.462689f,-0.120003f,0.0549334f},{-0.479924f,2.84331e-17f,-0.0575401f},{-0.427585f,-2.72736e-17f,-0.0121426f},
{-0.341068f,-1.15293e-16f,0.00656086f},{-0.148034f,-9.98777e-17f,0.018518f},{-0.143679f,-2.44984e-16f,0.0245059f},
{-0.129596f,-2.98737e-16f,0.0438707f},{0.0174899f,-2.00062e-16f,0.0386448f},{0.238021f,2.60275e-18f,0.0240336f},
{0.303194f,-1.05071e-16f,0.0150774f},{0.322143f,-1.67756e-16f,0.0139084f},{0.388982f,-2.66592e-17f,0.115319f},
{0.417918f,-4.86128e-16f,0.12262f},{0.476564f,-4.03408e-16f,0.0784426f},{0.5f,-1.12933e-16f,0.013456f},
{0.474259f,-7.62386e-17f,-0.0229682f},{0.101492f,8.98381e-17f,-0.101812f},{-0.0915774f,7.79108e-17f,-0.12262f},
{-0.288639f,-5.41611e-18f,-0.121988f},{-0.398171f,7.17474e-17f,-0.105662f},{-0.469626f,-0.0160377f,-0.0537283f},
{-0.417287f,-0.0160377f,-0.0134575f},{-0.331459f,-0.017214f,0.0031338f},{-0.148034f,-0.0336162f,0.0137405f},
{-0.143679f,-0.0336162f,0.0190629f},{-0.129596f,-0.0336162f,0.0362753f},{0.0174899f,-0.0255202f,0.0316188f},
{0.238021f,-0.0159638f,0.0186333f},{0.303194f,-0.00965057f,0.0106885f},{0.322143f,-0.00681857f,0.00965151f},
{0.388982f,-0.00681858f,0.0996101f},{0.417918f,-0.00681858f,0.106087f},{0.476564f,-0.00681857f,0.0668979f},
{0.5f,-0.00681857f,0.0092502f},{0.474259f,-0.00681857f,-0.0230606f},{0.101492f,-0.0138603f,-0.0930008f},
{-0.0898133f,-0.0160377f,-0.111459f},{-0.279031f,-0.0160377f,-0.110898f},{-0.387873f,-0.0160377f,-0.0964156f},
{-0.43187f,-0.0469295f,-0.0461322f},{-0.37953f,-0.0469295f,-0.0190703f},{-0.296229f,-0.0469295f,-0.00792091f},
{-0.17159f,-0.0469295f,-0.00159941f},{-0.143679f,-0.0469295f,0.000474672f},{-0.130166f,-0.0469295f,0.00440889f},
{0.0174899f,-0.0469295f,0.00594783f},{0.238021f,-0.0466523f,0.00249474f},{0.303194f,-0.0229778f,-0.00284412f},
{0.322143f,-0.0123578f,-0.003541f},{0.379588f,-0.0123578f,-0.00303904f},{0.424851f,-0.0123578f,-0.00347153f},
{0.458125f,-0.0123578f,-0.00227978f},{0.480473f,-0.0123578f,-0.00894988f},{0.45582f,-0.0123578f,-0.020914f},
{0.097137f,-0.0387642f,-0.0714346f},{-0.0833453f,-0.0469295f,-0.0849273f},{-0.243801f,-0.0469295f,-0.0845505f},
{-0.350116f,-0.0469295f,-0.0748181f},{-0.39068f,-0.0572268f,-0.0428162f},{-0.338341f,-0.0572268f,-0.036416f},
{-0.257797f,-0.0572268f,-0.0337792f},{-0.164534f,-0.0572268f,-0.0322842f},{-0.143679f,-0.0572268f,-0.0320511f},
{-0.130166f,-0.0572268f,-0.0319197f},{0.0174899f,-0.0572268f,-0.0260309f},{0.237467f,-0.0572268f,-0.0203327f},
{0.255289f,-0.0572268f,-0.0212221f},{0.252998f,-0.0572268f,-0.0212194f},{0.310443f,-0.0572268f,-0.0211007f},
{0.355706f,-0.0572268f,-0.021203f},{0.38898f,-0.0572268f,-0.0209212f},{0.411328f,-0.0572268f,-0.0224987f},
{0.386676f,-0.0572268f,-0.0253282f},{0.0808059f,-0.0572268f,-0.0460784f},{-0.0762893f,-0.0572268f,-0.0519912f},
{-0.205368f,-0.0572268f,-0.0519021f},{-0.308927f,-0.0572268f,-0.0496004f},{-0.469626f,0.0160377f,-0.0537283f},
{-0.417287f,0.0160377f,-0.0134575f},{-0.331459f,0.017214f,0.0031338f},{-0.148034f,0.0336162f,0.0137405f},
{-0.143679f,0.0336162f,0.0190629f},{-0.129596f,0.0336162f,0.0362753f},{0.0174899f,0.0255202f,0.0316188f},
{0.238021f,0.0159638f,0.0186333f},{0.303194f,0.00965057f,0.0106885f},{0.322143f,0.00681857f,0.00965151f},
{0.388982f,0.00681858f,0.0996101f},{0.417918f,0.00681858f,0.106087f},{0.476564f,0.00681857f,0.0668979f},
{0.5f,0.00681857f,0.0092502f},{0.474259f,0.00681857f,-0.0230606f},{0.101492f,0.0138603f,-0.0930008f},
{-0.0898133f,0.0160377f,-0.111459f},{-0.279031f,0.0160377f,-0.110898f},{-0.387873f,0.0160377f,-0.0964156f},
{-0.43187f,0.0469295f,-0.0461322f},{-0.37953f,0.0469295f,-0.0190703f},{-0.296229f,0.0469295f,-0.00792091f},
{-0.17159f,0.0469295f,-0.00159941f},{-0.143679f,0.0469295f,0.000474672f},{-0.130166f,0.0469295f,0.00440889f},
{0.0174899f,0.0469295f,0.00594783f},{0.238021f,0.0466523f,0.00249474f},{0.303194f,0.0229778f,-0.00284412f},
{0.322143f,0.0123578f,-0.003541f},{0.379588f,0.0123578f,-0.00303904f},{0.424851f,0.0123578f,-0.00347153f},
{0.458125f,0.0123578f,-0.00227978f},{0.480473f,0.0123578f,-0.00894988f},{0.45582f,0.0123578f,-0.020914f},
{0.097137f,0.0387642f,-0.0714346f},{-0.0833453f,0.0469295f,-0.0849273f},{-0.243801f,0.0469295f,-0.0845505f},
{-0.350116f,0.0469295f,-0.0748181f},{-0.39068f,0.0572268f,-0.0428162f},{-0.338341f,0.0572268f,-0.036416f},
{-0.257797f,0.0572268f,-0.0337792f},{-0.164534f,0.0572268f,-0.0322842f},{-0.143679f,0.0572268f,-0.0320511f},
{-0.130166f,0.0572268f,-0.0319197f},{0.0174899f,0.0572268f,-0.0260309f},{0.237467f,0.0572268f,-0.0203327f},
{0.255289f,0.0572268f,-0.0212221f},{0.252998f,0.0572268f,-0.0212194f},{0.310443f,0.0572268f,-0.0211007f},
{0.355706f,0.0572268f,-0.021203f},{0.38898f,0.0572268f,-0.0209212f},{0.411328f,0.0572268f,-0.0224987f},
{0.386676f,0.0572268f,-0.0253282f},{0.0808059f,0.0572268f,-0.0460784f},{-0.0762893f,0.0572268f,-0.0519912f},
{-0.205368f,0.0572268f,-0.0519021f},{-0.308927f,0.0572268f,-0.0496004f},{-0.39068f,0.0572268f,-0.0428162f},
{-0.338341f,0.0572268f,-0.036416f},{-0.257797f,0.0572268f,-0.0337792f},{-0.164534f,0.0572268f,-0.0322842f},
{-0.143679f,0.0572268f,-0.0320511f},{-0.130166f,0.0572268f,-0.0319197f},{0.0174899f,0.0572268f,-0.0260309f},
{0.237467f,0.0572268f,-0.0203327f},{0.255289f,0.0572268f,-0.0212221f},{0.252998f,0.0572268f,-0.0212194f},
{0.310443f,0.0572268f,-0.0211007f},{0.355706f,0.0572268f,-0.021203f},{0.38898f,0.0572268f,-0.0209212f},
{0.411328f,0.0572268f,-0.0224987f},{0.386676f,0.0572268f,-0.0253282f},{0.0808059f,0.0572268f,-0.0460784f},
{-0.0762893f,0.0572268f,-0.0519912f},{-0.205368f,0.0572268f,-0.0519021f},{-0.308927f,0.0572268f,-0.0496004f}
};
static GLfloat normals [366][3] = {
{-0.560155f,0.359424f,0.746352f},{-0.560155f,0.359422f,0.746352f},{-0.560154f,0.359424f,0.746352f},
{-0.560154f,0.359424f,0.746352f},{-0.560156f,0.35942f,0.746353f},{-0.560157f,0.359417f,0.746354f},
{-0.560155f,0.807619f,0.184334f},{-0.560155f,0.807617f,0.184337f},{-0.560154f,0.807619f,0.184334f},
{-0.560154f,0.807619f,0.184334f},{-0.560156f,0.807617f,0.184338f},{-0.560157f,0.807615f,0.184342f},
{-0.560155f,0.64766f,-0.516491f},{-0.560152f,0.647654f,-0.516501f},{-0.560154f,0.64766f,-0.516492f},
{-0.560151f,0.647652f,-0.516505f},{-0.560148f,0.647644f,-0.516519f},{-0.560155f,0.0f,-0.828388f},
{-0.560155f,0.0f,-0.828388f},{-0.560154f,0.0f,-0.828388f},{-0.560155f,0.0f,-0.828388f},
{-0.560154f,-0.64766f,-0.516491f},{-0.560157f,-0.647652f,-0.516499f},{-0.560154f,-0.64766f,-0.516492f},
{-0.560158f,-0.647648f,-0.516502f},{-0.560162f,-0.647636f,-0.516513f},{-0.560155f,-0.807619f,0.184334f},
{-0.560154f,-0.807618f,0.184337f},{-0.560155f,-0.807619f,0.184334f},{-0.560154f,-0.807619f,0.184334f},
{-0.560154f,-0.807618f,0.184338f},{-0.560153f,-0.807618f,0.184343f},{-0.560155f,-0.359424f,0.746352f},
{-0.560154f,-0.359422f,0.746353f},{-0.560154f,-0.359424f,0.746352f},{-0.560154f,-0.359424f,0.746352f},
{-0.560154f,-0.359421f,0.746354f},{-0.560153f,-0.359418f,0.746356f},{0.0f,-1.0f,0.0f},
{0.301016f,0.0f,0.953619f},{0.397229f,-0.00183164f,0.917718f},{0.532262f,-0.00296292f,0.846574f},
{0.571543f,0.00307038f,0.820567f},{0.0f,1.0f,0.0f},{-0.627821f,0.0f,0.778358f},
{-0.651705f,0.00511071f,0.758456f},{-0.627812f,0.00510577f,0.778348f},{-0.648893f,-0.00414451f,0.760868f},
{0.114792f,0.0f,0.99339f},{-0.00774874f,-0.00691356f,-0.999946f},{-0.00993694f,-0.0052495f,-0.999937f},
{-0.00742657f,0.00793026f,-0.999941f},{-0.00993659f,0.00983848f,-0.999902f},{0.315015f,0.0410275f,0.9482f},
{0.551432f,0.429561f,0.715123f},{0.114735f,0.0315347f,0.992896f},{-0.663417f,0.00766702f,0.748211f},
{-0.697605f,0.0f,0.716483f},{-0.00491551f,0.0158601f,-0.999862f},{0.000104337f,0.0120436f,-0.999927f},
{-0.659252f,-0.00621737f,0.751897f},{-0.689544f,-0.012424f,0.724137f},{0.252208f,-0.0263303f,0.967315f},
{0.114525f,-0.0681045f,0.991083f},{0.584069f,-0.00445307f,0.811692f},{-0.00555996f,-0.0138269f,-0.999889f},
{-0.00118323f,-0.0171544f,-0.999852f},{-0.467191f,-0.704221f,0.534608f},{-0.469911f,-0.731885f,0.493486f},
{-0.46332f,-0.674821f,0.574414f},{0.114193f,-0.102038f,0.988205f},{0.114193f,-0.102038f,0.988205f},
{0.441993f,-0.65085f,0.617281f},{0.463027f,-0.682611f,0.565374f},{0.419126f,-0.616391f,0.66663f},
{-0.00531028f,-0.30694f,-0.951714f},{-0.0011212f,-0.319934f,-0.947439f},{-0.00949827f,-0.293883f,-0.955794f},
{0.465153f,0.6106f,0.640937f},{0.480455f,0.668855f,0.56727f},{0.445639f,0.546817f,0.7088f},
{0.114278f,0.0945099f,0.988943f},{-0.48053f,0.689131f,0.542392f},{-0.501387f,0.695293f,0.514955f},
{-0.459085f,0.682124f,0.569164f},{-0.00463423f,0.318662f,-0.947857f},{9.94269e-05f,0.303275f,-0.952903f},
{-0.00936656f,0.333958f,-0.942541f},{-0.999951f,3.47803e-05f,0.00993729f},{-0.999409f,-0.000441089f,0.0343663f},
{-0.999951f,6.51912e-05f,0.00993729f},{-0.999951f,9.77868e-05f,0.00993729f},{0.999951f,6.51912e-05f,-0.00993729f},
{0.999951f,9.77868e-05f,-0.00993729f},{0.999951f,3.47803e-05f,-0.00993729f},{0.999409f,-0.00044109f,-0.0343663f},
{-0.726259f,-0.00827905f,-0.687371f},{0.999951f,0.000195574f,-0.00993729f},{-0.999951f,0.000195574f,0.00993729f},
{0.726256f,-0.00827909f,0.687375f},{-0.998914f,-0.000661684f,0.0465813f},{-0.996535f,-0.00142682f,0.0831626f},
{0.998914f,-0.000661684f,-0.0465813f},{0.996535f,-0.00142682f,-0.0831626f},{0.172638f,0.0f,0.984985f},
{0.234047f,-0.00201268f,0.972223f},{0.317539f,-0.00331872f,0.948239f},{0.349151f,0.00366367f,0.937059f},
{-0.137728f,0.0f,0.99047f},{-0.232154f,0.00372467f,0.972672f},{-0.394719f,0.0060268f,0.918782f},
{-0.414106f,-0.00510234f,0.910214f},{-0.00412731f,-0.00691364f,-0.999968f},{-0.00529287f,-0.0052497f,-0.999972f},
{-0.0039557f,0.00793037f,-0.999961f},{-0.00529269f,0.00983883f,-0.999938f},{0.184922f,0.0407128f,0.98191f},
{0.411181f,0.0109785f,0.911488f},{-0.0710674f,0.0244192f,0.997173f},{-0.427813f,0.00904621f,0.903822f},
{-0.170028f,0.00728359f,-0.985412f},{5.55992e-05f,0.0120436f,-0.999927f},{-0.136914f,-0.0454765f,0.989539f},
{-0.452275f,-0.0152997f,0.891747f},{0.147781f,-0.0259515f,0.98868f},{0.359026f,-0.00498293f,0.933314f},
{-0.00296146f,-0.013827f,-0.9999f},{-0.000630229f,-0.0171545f,-0.999853f},{-0.270918f,-0.766745f,0.581984f},
{-0.272788f,-0.797684f,0.537853f},{-0.26826f,-0.733576f,0.624422f},{0.061108f,-0.102518f,0.992852f},
{0.0611081f,-0.102518f,0.992852f},{0.253964f,-0.702137f,0.665211f},{0.268059f,-0.741956f,0.614529f},
{0.238763f,-0.659263f,0.712997f},{-0.00282835f,-0.306943f,-0.951724f},{-0.000597192f,-0.319934f,-0.94744f},
{-0.00505897f,-0.293893f,-0.955825f},{0.210868f,0.50022f,0.83983f},{0.280105f,0.732116f,0.620925f},
{0.269597f,0.664724f,0.696749f},{0.256285f,0.590419f,0.765326f},{0.061154f,0.0949554f,0.993601f},
{-0.280286f,0.754453f,0.593498f},{-0.29492f,0.767857f,0.568699f},{-0.265369f,0.740289f,0.617699f},
{-0.00246836f,0.318665f,-0.947864f},{-0.256387f,0.153151f,-0.954364f},{-0.00498894f,0.333968f,-0.942571f},
{-0.999826f,6.52755e-05f,0.0186502f},{-0.99794f,-0.000821486f,0.0641522f},{-0.999826f,0.00012235f,0.0186502f},
{-0.999826f,0.000183525f,0.0186502f},{0.999826f,0.000122486f,-0.0186709f},{0.999826f,0.000183729f,-0.0186709f},
{0.999826f,6.5348e-05f,-0.0186709f},{0.997935f,-0.000822379f,-0.0642224f},{-0.490188f,-0.0104975f,-0.871553f},
{0.999826f,0.000367458f,-0.0186709f},{-0.999826f,0.000367051f,0.0186502f},{0.490418f,-0.0104958f,0.871424f},
{-0.996216f,-0.00123255f,0.0869065f},{-0.987949f,-0.00265516f,0.154757f},{0.996207f,-0.00123389f,-0.0870014f},
{0.987923f,-0.00265804f,-0.154924f},{-0.579141f,-0.379839f,0.721331f},{-0.348223f,-0.405761f,0.845044f},
{-0.430572f,0.0130697f,0.902462f},{-0.633691f,0.138688f,0.761053f},{-0.118389f,-0.327653f,0.937351f},
{-0.136341f,-0.00926631f,0.990619f},{-0.0676951f,-0.453215f,0.888827f},{-0.0644956f,-0.0766324f,0.994971f},
{-0.788268f,-0.0926903f,0.608311f},{-0.655082f,-0.400683f,0.640563f},{-0.797399f,0.00811836f,0.603398f},
{-0.796969f,0.0338417f,0.603071f},{-0.702499f,-0.419647f,0.574798f},{-0.79696f,-0.0351665f,0.603008f},
{0.0398721f,-0.24278f,0.969262f},{0.0543431f,-0.588233f,0.806864f},{0.0533654f,0.017231f,0.998426f},
{0.0382088f,0.0906049f,0.995154f},{0.0992063f,-0.464822f,0.879829f},{0.0875138f,-0.30102f,0.949594f},
{0.156666f,-0.540025f,0.826939f},{0.111725f,-0.380876f,0.917851f},{0.256546f,-0.711877f,0.65377f},
{0.0905777f,-0.470674f,0.877646f},{-0.467228f,-0.813134f,0.347148f},{-0.195453f,-0.942057f,0.27263f},
{-0.29173f,-0.877412f,0.380843f},{-0.789654f,-0.324923f,0.520453f},{0.09223f,-0.96677f,0.23843f},
{0.0308277f,-0.921654f,0.386786f},{0.12142f,-0.977855f,0.170462f},{0.299676f,-0.85411f,0.425077f},
{0.86849f,-0.366158f,0.334147f},{0.902346f,-0.226272f,0.366842f},{0.920424f,-0.181212f,0.346384f},
{0.815714f,-0.498069f,0.294173f},{0.786964f,0.174393f,-0.59184f},{0.78211f,0.00844444f,-0.623083f},
{0.800016f,0.113507f,-0.589144f},{0.805693f,-0.0108572f,-0.592233f},{0.298255f,-0.590009f,-0.750289f},
{0.11964f,-0.615783f,-0.77878f},{0.155553f,0.109712f,-0.981716f},{0.19562f,0.182266f,-0.963593f},
{0.0407678f,-0.610579f,-0.790905f},{0.0579557f,-0.556754f,-0.828653f},{-0.0546849f,-0.62518f,-0.778563f},
{-0.0406572f,-0.585174f,-0.809888f},{-0.234101f,-0.634736f,-0.736415f},{-0.208683f,-0.60545f,-0.768038f},
{-0.335495f,-0.594247f,-0.730968f},{-0.404719f,-0.547427f,-0.732479f},{-0.394649f,-0.651906f,0.647511f},
{-0.180196f,-0.719972f,0.670201f},{-0.0575526f,-0.776592f,0.627369f},{-0.0251965f,-0.889498f,0.456244f},
{-0.146146f,-0.902311f,0.405556f},{-0.651954f,0.538887f,0.533439f},{-0.0251156f,-0.924452f,0.380471f},
{-0.0246746f,-0.922272f,0.385753f},{0.0121047f,-0.847208f,0.531124f},{0.142594f,-0.765196f,0.627808f},
{0.30841f,-0.764969f,0.565426f},{0.131965f,-0.949012f,0.286289f},{-0.00409991f,-0.998578f,0.0531536f},
{0.010937f,-0.998061f,0.0612794f},{0.0609262f,-0.991135f,0.118061f},{0.0668973f,-0.972259f,0.224137f},
{0.0928424f,-0.986261f,0.136636f},{0.441611f,-0.642454f,-0.626285f},{0.209991f,-0.51026f,-0.83399f},
{0.637782f,-0.578851f,-0.5081f},{0.101106f,-0.648717f,-0.754284f},{0.0295392f,-0.805712f,-0.59157f},
{-0.0333122f,-0.84156f,-0.539136f},{-0.134608f,-0.836982f,-0.530417f},{-0.17529f,-0.54466f,-0.820134f},
{-0.150857f,-0.879134f,0.452069f},{-0.03921f,-0.918092f,0.394424f},{-0.0114399f,-0.940422f,0.339816f},
{-0.0105785f,-0.950436f,0.31074f},{-0.0312142f,-0.955911f,0.291993f},{-0.00590682f,-0.958804f,0.284006f},
{-0.00571651f,-0.938553f,0.345088f},{0.131465f,-0.801043f,0.583992f},{0.0426291f,-0.518039f,0.854294f},
{-0.00106269f,0.367992f,-0.929829f},{0.474288f,-0.82634f,0.303667f},{-0.00504293f,-0.363274f,0.931669f},
{0.00299925f,-0.375437f,0.926843f},{-0.000422581f,-0.370571f,0.928804f},{-0.00816648f,-0.355644f,0.934586f},
{-0.013222f,-0.355265f,0.934672f},{-0.00766236f,-0.359247f,0.933211f},{0.0936904f,-0.472872f,0.876136f},
{0.0604021f,-0.455545f,0.888161f},{0.0651562f,-0.379134f,0.923045f},{0.144482f,-0.519336f,0.842268f},
{0.266046f,-0.230757f,-0.935933f},{0.130774f,-0.104538f,-0.985885f},{0.0387866f,-0.81952f,-0.571736f},
{0.0434223f,-0.876241f,-0.479912f},{0.00346864f,-0.953992f,-0.299812f},{-0.0111484f,-0.949384f,-0.313919f},
{-0.048109f,-0.949765f,-0.309244f},{-0.0692841f,-0.546006f,-0.834912f},{-0.0878171f,-0.964184f,-0.250275f},
{-0.370729f,0.407137f,0.834745f},{-0.510128f,0.407371f,0.757508f},{-0.133461f,0.392707f,0.909928f},
{-0.187832f,0.521058f,0.832597f},{-0.782232f,0.0955148f,0.615621f},{-0.770829f,0.0896341f,0.630705f},
{-0.7862f,0.116927f,0.606809f},{0.0504229f,0.476586f,0.877681f},{0.0429707f,0.220186f,0.974511f},
{0.124671f,0.42764f,0.89531f},{0.110768f,0.349823f,0.930244f},{0.18892f,0.595847f,0.780561f},
{0.0960242f,0.451432f,0.887124f},{0.0818397f,0.471351f,0.87814f},{0.0522611f,0.528853f,0.847103f},
{-0.659279f,0.589045f,0.467308f},{-0.75224f,0.348933f,0.558911f},{-0.239254f,0.893136f,0.380876f},
{0.0161202f,0.974541f,0.22363f},{-0.0460226f,0.988777f,0.142136f},{0.173302f,0.892324f,0.416804f},
{0.293415f,0.923857f,0.245755f},{0.732857f,0.550291f,0.400126f},{0.865152f,0.377437f,0.330232f},
{0.7863f,-0.186357f,-0.58907f},{0.730073f,-0.358753f,-0.581626f},{0.131704f,0.519174f,-0.84446f},
{0.184147f,-0.0133197f,-0.982808f},{0.0418404f,0.60327f,-0.796439f},{0.0278711f,0.568399f,-0.822281f},
{-0.0465214f,0.624761f,-0.779429f},{-0.0768685f,0.577977f,-0.812425f},{-0.215233f,0.63699f,-0.740215f},
{-0.31442f,0.557654f,-0.76822f},{-0.392362f,0.573656f,-0.719007f},{-0.448105f,0.468668f,-0.761283f},
{-0.220345f,0.748374f,0.625608f},{-0.28223f,0.248523f,0.926597f},{-0.114475f,0.41349f,0.903284f},
{-0.0427226f,0.814571f,0.578488f},{-0.156961f,0.876184f,0.455702f},{-0.401889f,0.781138f,0.477817f},
{-0.118489f,0.922271f,0.367937f},{-0.129301f,0.869752f,0.476249f},{0.0111474f,0.889985f,0.455853f},
{0.116347f,0.741285f,0.661029f},{0.327644f,0.706631f,0.627154f},{0.185428f,0.740904f,0.645506f},
{0.024167f,0.959166f,0.281811f},{-0.0288102f,0.985076f,0.169691f},{-0.00414468f,0.998673f,0.0513369f},
{0.0218499f,0.997708f,0.0640398f},{0.111906f,0.990644f,0.078116f},{0.232658f,0.970504f,-0.0631862f},
{0.251765f,-0.816998f,-0.518776f},{0.192381f,0.811697f,-0.551486f},{0.239637f,0.790335f,-0.563865f},
{0.102855f,0.791899f,-0.601927f},{0.0398918f,0.833637f,-0.550871f},{-0.0186367f,0.836819f,-0.547163f},
{-0.106763f,0.82051f,-0.561574f},{-0.257771f,0.741234f,-0.61978f},{-0.119742f,0.163621f,0.979229f},
{-0.0745542f,0.92516f,0.372184f},{-0.13197f,0.957829f,0.255238f},{-0.0213178f,0.921493f,0.38781f},
{-0.0213183f,0.943434f,0.330875f},{-0.00844381f,0.94336f,0.331664f},{-0.009636f,0.951656f,0.307016f},
{-0.0310064f,0.958821f,0.282315f},{-0.00733745f,0.955385f,0.295272f},{0.00692157f,0.923798f,0.382817f},
{0.174084f,0.846005f,0.503954f},{-0.00100639f,-0.473919f,-0.880568f},{0.479958f,0.839714f,0.254009f},
{-0.00265826f,0.367608f,0.929977f},{-0.0019246f,0.364005f,0.931395f},{0.000930346f,0.372704f,0.92795f},
{0.00100748f,0.370832f,0.928699f},{-0.0109192f,0.358432f,0.933492f},{0.00695453f,0.387396f,0.921887f},
{0.0877728f,0.470386f,0.878085f},{0.147739f,0.530287f,0.834847f},{0.230988f,0.589657f,0.773918f},
{0.0906343f,-0.0587276f,-0.994151f},{0.113155f,-0.123328f,-0.985894f},{0.376969f,0.504508f,-0.776766f},
{0.0676833f,0.00615119f,-0.997688f},{0.0789454f,0.829393f,-0.553059f},{0.0295621f,0.915839f,-0.400456f},
{-0.00248009f,0.95475f,-0.297398f},{-0.0217485f,0.946389f,-0.322295f},{-0.101347f,0.902393f,-0.418827f}
};
GLfloat textures[1][2]={{0.0f,0.0f}};
/*Material indicies*/
/*{material index,face count}*/
static int material_ref [1][2] = {
{0,410}
};
void MyMaterial(GLenum mode,GLfloat *f,GLfloat alpha)
{
  GLfloat d[4];
  d[0]=f[0];
  d[1]=f[1];
  d[2]=f[2];
  d[3]=alpha;
  glMaterialfv (GL_FRONT_AND_BACK,mode,d);
}
/*
 *  SelectMaterial uses OpenGL commands to define facet colors.
 *
 *  Returns:
 *    Nothing
 */

void SelectMaterial(int i)
{
  //
  // Define the reflective properties of the 3D Object faces.
  //
  glEnd();
  GLfloat alpha=materials[i].alpha;
  MyMaterial (GL_AMBIENT, materials[i].ambient,alpha);
  MyMaterial (GL_DIFFUSE, materials[i].diffuse,alpha);
  MyMaterial (GL_SPECULAR, materials[i].specular,alpha);
  MyMaterial (GL_EMISSION, materials[i].emission,alpha);
  glMaterialf (GL_FRONT_AND_BACK,GL_SHININESS,materials[i].phExp);
  glBegin(GL_TRIANGLES);

};

GLuint Gen3DObjectList()
{
 int i;
 int j;

 GLuint lid=glGenLists(1);
	int mcount=0;
	int mindex=0;
   glNewList(lid, GL_COMPILE);

    glBegin (GL_TRIANGLES);
      for(i=0;i<sizeof(face_indicies)/sizeof(face_indicies[0]);i++)
       {
      if(!mcount)
       {
        SelectMaterial(material_ref[mindex][0]);
        mcount=material_ref[mindex][1];
        mindex++;
       }
       mcount--;
       for(j=0;j<3;j++)
        {
          int vi=face_indicies[i][j];
          int ni=face_indicies[i][j+3];//Normal index
          int ti=face_indicies[i][j+6];//Texture index
           glNormal3f (normals[ni][0],normals[ni][1],normals[ni][2]);
           glTexCoord2f(textures[ti][0],textures[ti][1]);
           glVertex3f (vertices[vi][0],vertices[vi][1],vertices[vi][2]);
        }
       }
    glEnd ();

   glEndList();
   return lid;
};

//plane =  Gen3DObjectList();

#endif
