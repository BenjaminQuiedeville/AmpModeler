/* ------------------------------------------------------------
author: "FDN_Seeker"
name: "TableSaw"
version: "0.0.1"
Code generated with Faust 2.79.3 (https://faust.grame.fr)
Compilation options: -lang cpp -nvi -ct 1 -cn TablesawDSP -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0 -vec -lv 0 -vs 32
------------------------------------------------------------ */

#ifndef  __TablesawDSP_H__
#define  __TablesawDSP_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/* link with : "" */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS TablesawDSP
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

static float TablesawDSP_faustpower2_f(float value) {
	return value * value;
}

class TablesawDSP final : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fConst2;
	float fConst3;
	FAUSTFLOAT fHslider0;
	float fYec0_perm[4];
	float fConst4;
	float fConst5;
	float fRec5_perm[4];
	float fYec1_perm[4];
	float fConst6;
	float fConst7;
	float fConst8;
	float fRec4_perm[4];
	float fConst9;
	float fConst10;
	float fConst11;
	float fRec3_perm[4];
	FAUSTFLOAT fHslider1;
	float fConst12;
	float fConst13;
	float fConst14;
	float fConst15;
	float fRec2_perm[4];
	FAUSTFLOAT fHslider2;
	float fConst16;
	float fConst17;
	float fConst18;
	float fConst19;
	float fRec1_perm[4];
	float fConst20;
	float fConst21;
	float fConst22;
	float fConst23;
	float fRec0_perm[4];
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	
 public:
	TablesawDSP() {
	}
	
	void metadata(Meta* m) { 
		m->declare("author", "FDN_Seeker");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.21.0");
		m->declare("compile_options", "-lang cpp -nvi -ct 1 -cn TablesawDSP -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0 -vec -lv 0 -vs 32");
		m->declare("description", "Boss HM2 Emulation");
		m->declare("filename", "Tablesaw.dsp");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/deprecated", "This library is deprecated and is not maintained anymore. It will be removed in August 2017.");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("filter.lib/version", "1.29");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/deprecated", "This library is deprecated and is not maintained anymore. It will be removed in August 2017.");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/version", "1.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.8.1");
		m->declare("misceffects.lib/dryWetMixerConstantPower:author", "David Braun, revised by Stéphane Letz");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.5.1");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/deprecated", "This library is deprecated and is not maintained anymore. It will be removed in August 2017.");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/version", "1.0");
		m->declare("name", "TableSaw");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "1.6.0");
		m->declare("version", "0.0.1");
	}

	static constexpr int getStaticNumInputs() {
		return 1;
	}
	static constexpr int getStaticNumOutputs() {
		return 1;
	}
	int getNumInputs() {
		return 1;
	}
	int getNumOutputs() {
		return 1;
	}
	
	static void classInit(int sample_rate) {
	}
	
	void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = std::tan(31.415926f / fConst0);
		fConst2 = 1.0f / fConst1;
		fConst3 = 1.0f - fConst2;
		fConst4 = 1.5f / fConst1;
		fConst5 = 1.0f / (fConst2 + 1.0f);
		fConst6 = 1.0f / std::tan(37699.113f / fConst0);
		fConst7 = 1.0f - fConst6;
		fConst8 = 1.0f / (fConst6 + 1.0f);
		fConst9 = 1.0f / std::tan(94.24778f / fConst0);
		fConst10 = 1.0f - fConst9;
		fConst11 = 1.0f / (fConst9 + 1.0f);
		fConst12 = 251.32741f / (fConst0 * std::sin(251.32741f / fConst0));
		fConst13 = std::tan(125.663704f / fConst0);
		fConst14 = 1.0f / fConst13;
		fConst15 = 2.0f * (1.0f - 1.0f / TablesawDSP_faustpower2_f(fConst13));
		fConst16 = 4712.389f / (fConst0 * std::sin(4712.389f / fConst0));
		fConst17 = std::tan(2356.1946f / fConst0);
		fConst18 = 1.0f / fConst17;
		fConst19 = 2.0f * (1.0f - 1.0f / TablesawDSP_faustpower2_f(fConst17));
		fConst20 = 3141.5928f / (fConst0 * std::sin(3141.5928f / fConst0));
		fConst21 = std::tan(1570.7964f / fConst0);
		fConst22 = 1.0f / fConst21;
		fConst23 = 2.0f * (1.0f - 1.0f / TablesawDSP_faustpower2_f(fConst21));
	}
	
	void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.5f);
		fHslider1 = FAUSTFLOAT(0.5f);
		fHslider2 = FAUSTFLOAT(0.5f);
		fHslider3 = FAUSTFLOAT(1.0f);
		fHslider4 = FAUSTFLOAT(0.5f);
	}
	
	void instanceClear() {
		for (int l0 = 0; l0 < 4; l0 = l0 + 1) {
			fYec0_perm[l0] = 0.0f;
		}
		for (int l1 = 0; l1 < 4; l1 = l1 + 1) {
			fRec5_perm[l1] = 0.0f;
		}
		for (int l2 = 0; l2 < 4; l2 = l2 + 1) {
			fYec1_perm[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 4; l3 = l3 + 1) {
			fRec4_perm[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 4; l4 = l4 + 1) {
			fRec3_perm[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 4; l5 = l5 + 1) {
			fRec2_perm[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 4; l6 = l6 + 1) {
			fRec1_perm[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 4; l7 = l7 + 1) {
			fRec0_perm[l7] = 0.0f;
		}
	}
	
	void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	TablesawDSP* clone() {
		return new TablesawDSP();
	}
	
	int getSampleRate() {
		return fSampleRate;
	}
	
	void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("TableSaw");
		ui_interface->addHorizontalSlider("drywet", &fHslider3, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("gain", &fHslider0, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f));
		ui_interface->addHorizontalSlider("high", &fHslider2, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f));
		ui_interface->addHorizontalSlider("low", &fHslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f));
		ui_interface->addHorizontalSlider("vol", &fHslider4, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
	}
	
	void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0_ptr = inputs[0];
		FAUSTFLOAT* output0_ptr = outputs[0];
		float fSlow0 = 0.6666667f * std::pow(1e+01f, 3.0f * float(fHslider0));
		float fZec0[32];
		float fYec0_tmp[36];
		float* fYec0 = &fYec0_tmp[4];
		float fRec5_tmp[36];
		float* fRec5 = &fRec5_tmp[4];
		float fYec1_tmp[36];
		float* fYec1 = &fYec1_tmp[4];
		float fRec4_tmp[36];
		float* fRec4 = &fRec4_tmp[4];
		float fRec3_tmp[36];
		float* fRec3 = &fRec3_tmp[4];
		float fSlow1 = 2e+01f * float(fHslider1);
		int iSlow2 = fSlow1 > 0.0f;
		float fSlow3 = fConst12 * std::pow(1e+01f, 0.05f * std::fabs(fSlow1));
		float fSlow4 = ((iSlow2) ? fConst12 : fSlow3);
		float fSlow5 = fConst14 * (fConst14 + fSlow4) + 1.0f;
		float fZec1[32];
		float fSlow6 = fConst14 * (fConst14 - fSlow4) + 1.0f;
		float fRec2_tmp[36];
		float* fRec2 = &fRec2_tmp[4];
		float fSlow7 = 2e+01f * float(fHslider2);
		int iSlow8 = fSlow7 > 0.0f;
		float fSlow9 = std::pow(1e+01f, 0.05f * std::fabs(fSlow7));
		float fSlow10 = fConst16 * fSlow9;
		float fSlow11 = ((iSlow8) ? fConst16 : fSlow10);
		float fSlow12 = fConst18 * (fConst18 + fSlow11) + 1.0f;
		float fZec2[32];
		float fSlow13 = fConst18 * (fConst18 - fSlow11) + 1.0f;
		float fSlow14 = ((iSlow2) ? fSlow3 : fConst12);
		float fSlow15 = fConst14 * (fConst14 - fSlow14) + 1.0f;
		float fSlow16 = fConst14 * (fConst14 + fSlow14) + 1.0f;
		float fRec1_tmp[36];
		float* fRec1 = &fRec1_tmp[4];
		float fSlow17 = fConst20 * fSlow9;
		float fSlow18 = ((iSlow8) ? fConst20 : fSlow17);
		float fSlow19 = fConst22 * (fConst22 + fSlow18) + 1.0f;
		float fZec3[32];
		float fSlow20 = fConst22 * (fConst22 - fSlow18) + 1.0f;
		float fSlow21 = ((iSlow8) ? fSlow10 : fConst16);
		float fSlow22 = fConst18 * (fConst18 - fSlow21) + 1.0f;
		float fSlow23 = fConst18 * (fConst18 + fSlow21) + 1.0f;
		float fRec0_tmp[36];
		float* fRec0 = &fRec0_tmp[4];
		float fSlow24 = 1.5707964f * float(fHslider3);
		float fSlow25 = 0.70710677f * std::cos(fSlow24);
		float fSlow26 = ((iSlow8) ? fSlow17 : fConst20);
		float fSlow27 = fConst22 * (fConst22 - fSlow26) + 1.0f;
		float fSlow28 = fConst22 * (fConst22 + fSlow26) + 1.0f;
		float fSlow29 = 0.2236068f * std::sin(fSlow24) * TablesawDSP_faustpower2_f(float(fHslider4));
		int vindex = 0;
		/* Main loop */
		for (vindex = 0; vindex <= (count - 32); vindex = vindex + 32) {
			FAUSTFLOAT* input0 = &input0_ptr[vindex];
			FAUSTFLOAT* output0 = &output0_ptr[vindex];
			int vsize = 32;
			/* Vectorizable loop 0 */
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec0[i] = fSlow0 * float(input0[i]);
			}
			/* Vectorizable loop 1 */
			/* Pre code */
			for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
				fYec0_tmp[j0] = fYec0_perm[j0];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fYec0[i] = tanhf(std::min<float>(0.0f, fZec0[i])) + std::max<float>(0.0f, std::min<float>(1.0f, fZec0[i])) + tanhf(std::max<float>(1.0f, fZec0[i]) + -1.0f);
			}
			/* Post code */
			for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
				fYec0_perm[j1] = fYec0_tmp[vsize + j1];
			}
			/* Recursive loop 2 */
			/* Pre code */
			for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
				fRec5_tmp[j2] = fRec5_perm[j2];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec5[i] = fConst5 * (fConst4 * (fYec0[i] - fYec0[i - 1]) - fConst3 * fRec5[i - 1]);
			}
			/* Post code */
			for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
				fRec5_perm[j3] = fRec5_tmp[vsize + j3];
			}
			/* Vectorizable loop 3 */
			/* Pre code */
			for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
				fYec1_tmp[j4] = fYec1_perm[j4];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fYec1[i] = std::max<float>(-0.7f, std::min<float>(0.7f, std::max<float>(0.01f, fRec5[i]) + std::min<float>(-0.01f, fRec5[i])));
			}
			/* Post code */
			for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
				fYec1_perm[j5] = fYec1_tmp[vsize + j5];
			}
			/* Recursive loop 4 */
			/* Pre code */
			for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
				fRec4_tmp[j6] = fRec4_perm[j6];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec4[i] = -(fConst8 * (fConst7 * fRec4[i - 1] - (fYec1[i] + fYec1[i - 1])));
			}
			/* Post code */
			for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
				fRec4_perm[j7] = fRec4_tmp[vsize + j7];
			}
			/* Recursive loop 5 */
			/* Pre code */
			for (int j8 = 0; j8 < 4; j8 = j8 + 1) {
				fRec3_tmp[j8] = fRec3_perm[j8];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec3[i] = -(fConst11 * (fConst10 * fRec3[i - 1] - fConst9 * (fRec4[i] - fRec4[i - 1])));
			}
			/* Post code */
			for (int j9 = 0; j9 < 4; j9 = j9 + 1) {
				fRec3_perm[j9] = fRec3_tmp[vsize + j9];
			}
			/* Recursive loop 6 */
			/* Pre code */
			for (int j10 = 0; j10 < 4; j10 = j10 + 1) {
				fRec2_tmp[j10] = fRec2_perm[j10];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec1[i] = fConst15 * fRec2[i - 1];
				fRec2[i] = fRec3[i] - (fRec2[i - 2] * fSlow6 + fZec1[i]) / fSlow5;
			}
			/* Post code */
			for (int j11 = 0; j11 < 4; j11 = j11 + 1) {
				fRec2_perm[j11] = fRec2_tmp[vsize + j11];
			}
			/* Recursive loop 7 */
			/* Pre code */
			for (int j12 = 0; j12 < 4; j12 = j12 + 1) {
				fRec1_tmp[j12] = fRec1_perm[j12];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec2[i] = fConst19 * fRec1[i - 1];
				fRec1[i] = (fZec1[i] + fRec2[i] * fSlow16 + fRec2[i - 2] * fSlow15) / fSlow5 - (fRec1[i - 2] * fSlow13 + fZec2[i]) / fSlow12;
			}
			/* Post code */
			for (int j13 = 0; j13 < 4; j13 = j13 + 1) {
				fRec1_perm[j13] = fRec1_tmp[vsize + j13];
			}
			/* Recursive loop 8 */
			/* Pre code */
			for (int j14 = 0; j14 < 4; j14 = j14 + 1) {
				fRec0_tmp[j14] = fRec0_perm[j14];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec3[i] = fConst23 * fRec0[i - 1];
				fRec0[i] = (fZec2[i] + fRec1[i] * fSlow23 + fRec1[i - 2] * fSlow22) / fSlow12 - (fRec0[i - 2] * fSlow20 + fZec3[i]) / fSlow19;
			}
			/* Post code */
			for (int j15 = 0; j15 < 4; j15 = j15 + 1) {
				fRec0_perm[j15] = fRec0_tmp[vsize + j15];
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				output0[i] = FAUSTFLOAT(fSlow29 * ((fZec3[i] + fRec0[i] * fSlow28 + fRec0[i - 2] * fSlow27) / fSlow19) + fSlow25 * float(input0[i]));
			}
		}
		/* Remaining frames */
		if (vindex < count) {
			FAUSTFLOAT* input0 = &input0_ptr[vindex];
			FAUSTFLOAT* output0 = &output0_ptr[vindex];
			int vsize = count - vindex;
			/* Vectorizable loop 0 */
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec0[i] = fSlow0 * float(input0[i]);
			}
			/* Vectorizable loop 1 */
			/* Pre code */
			for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
				fYec0_tmp[j0] = fYec0_perm[j0];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fYec0[i] = tanhf(std::min<float>(0.0f, fZec0[i])) + std::max<float>(0.0f, std::min<float>(1.0f, fZec0[i])) + tanhf(std::max<float>(1.0f, fZec0[i]) + -1.0f);
			}
			/* Post code */
			for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
				fYec0_perm[j1] = fYec0_tmp[vsize + j1];
			}
			/* Recursive loop 2 */
			/* Pre code */
			for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
				fRec5_tmp[j2] = fRec5_perm[j2];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec5[i] = fConst5 * (fConst4 * (fYec0[i] - fYec0[i - 1]) - fConst3 * fRec5[i - 1]);
			}
			/* Post code */
			for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
				fRec5_perm[j3] = fRec5_tmp[vsize + j3];
			}
			/* Vectorizable loop 3 */
			/* Pre code */
			for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
				fYec1_tmp[j4] = fYec1_perm[j4];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fYec1[i] = std::max<float>(-0.7f, std::min<float>(0.7f, std::max<float>(0.01f, fRec5[i]) + std::min<float>(-0.01f, fRec5[i])));
			}
			/* Post code */
			for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
				fYec1_perm[j5] = fYec1_tmp[vsize + j5];
			}
			/* Recursive loop 4 */
			/* Pre code */
			for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
				fRec4_tmp[j6] = fRec4_perm[j6];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec4[i] = -(fConst8 * (fConst7 * fRec4[i - 1] - (fYec1[i] + fYec1[i - 1])));
			}
			/* Post code */
			for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
				fRec4_perm[j7] = fRec4_tmp[vsize + j7];
			}
			/* Recursive loop 5 */
			/* Pre code */
			for (int j8 = 0; j8 < 4; j8 = j8 + 1) {
				fRec3_tmp[j8] = fRec3_perm[j8];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fRec3[i] = -(fConst11 * (fConst10 * fRec3[i - 1] - fConst9 * (fRec4[i] - fRec4[i - 1])));
			}
			/* Post code */
			for (int j9 = 0; j9 < 4; j9 = j9 + 1) {
				fRec3_perm[j9] = fRec3_tmp[vsize + j9];
			}
			/* Recursive loop 6 */
			/* Pre code */
			for (int j10 = 0; j10 < 4; j10 = j10 + 1) {
				fRec2_tmp[j10] = fRec2_perm[j10];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec1[i] = fConst15 * fRec2[i - 1];
				fRec2[i] = fRec3[i] - (fRec2[i - 2] * fSlow6 + fZec1[i]) / fSlow5;
			}
			/* Post code */
			for (int j11 = 0; j11 < 4; j11 = j11 + 1) {
				fRec2_perm[j11] = fRec2_tmp[vsize + j11];
			}
			/* Recursive loop 7 */
			/* Pre code */
			for (int j12 = 0; j12 < 4; j12 = j12 + 1) {
				fRec1_tmp[j12] = fRec1_perm[j12];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec2[i] = fConst19 * fRec1[i - 1];
				fRec1[i] = (fZec1[i] + fRec2[i] * fSlow16 + fRec2[i - 2] * fSlow15) / fSlow5 - (fRec1[i - 2] * fSlow13 + fZec2[i]) / fSlow12;
			}
			/* Post code */
			for (int j13 = 0; j13 < 4; j13 = j13 + 1) {
				fRec1_perm[j13] = fRec1_tmp[vsize + j13];
			}
			/* Recursive loop 8 */
			/* Pre code */
			for (int j14 = 0; j14 < 4; j14 = j14 + 1) {
				fRec0_tmp[j14] = fRec0_perm[j14];
			}
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				fZec3[i] = fConst23 * fRec0[i - 1];
				fRec0[i] = (fZec2[i] + fRec1[i] * fSlow23 + fRec1[i - 2] * fSlow22) / fSlow12 - (fRec0[i - 2] * fSlow20 + fZec3[i]) / fSlow19;
			}
			/* Post code */
			for (int j15 = 0; j15 < 4; j15 = j15 + 1) {
				fRec0_perm[j15] = fRec0_tmp[vsize + j15];
			}
			/* Vectorizable loop 9 */
			/* Compute code */
			for (int i = 0; i < vsize; i = i + 1) {
				output0[i] = FAUSTFLOAT(fSlow29 * ((fZec3[i] + fRec0[i] * fSlow28 + fRec0[i - 2] * fSlow27) / fSlow19) + fSlow25 * float(input0[i]));
			}
		}
	}

};

#endif
