#define WAVESHAPE_TABLE_SIZE 1201
static const float table_min = -5;
static const float table_max = 7.0;

static float waveshaping_table[WAVESHAPE_TABLE_SIZE] = {
-2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -1.9999667037037039f, -1.9998669629629628f, -1.999701f, -1.999469037037037f, -1.9991712962962964f, -1.9988080000000001f, -1.9983793703703705f, -1.99788562962963f, -1.9973269999999999f, -1.9967037037037036f, -1.9960159629629628f, -1.9952640000000001f, -1.994448037037037f, -1.9935682962962962f, -1.9926249999999999f, -1.9916183703703703f, -1.9905486296296298f, -1.9894159999999999f, -1.9882207037037036f, -1.986962962962963f, -1.9856429999999998f, -1.984261037037037f, -1.9828172962962962f, -1.981312f, -1.9797453703703705f, -1.9781176296296294f, -1.9764290000000002f, -1.9746797037037038f, -1.972869962962963f, -1.971f, -1.9690700370370369f, -1.967080296296296f, -1.9650310000000002f, -1.9629223703703704f, -1.9607546296296299f, -1.958528f, -1.9562427037037036f, -1.953898962962963f, -1.9514969999999998f, -1.9490370370370371f, -1.9465192962962963f, -1.943944f, -1.9413113703703702f, -1.9386216296296297f, -1.9358749999999998f, -1.9330717037037035f, -1.9302119629629628f, -1.9272960000000001f, -1.924324037037037f, -1.9212962962962965f, -1.9182130000000002f, -1.9150743703703703f, -1.9118806296296296f, -1.908632f, -1.9053287037037039f, -1.9019709629629629f, -1.898559f, -1.895093037037037f, -1.8915732962962961f, -1.888f, -1.8843733703703702f, -1.8806936296296295f, -1.876961f, -1.8731757037037036f, -1.8693379629629632f, -1.8654479999999998f, -1.861506037037037f, -1.8575122962962962f, -1.8534670000000002f, -1.8493703703703703f, -1.8452226296296295f, -1.8410239999999998f, -1.8367747037037039f, -1.8324749629629626f, -1.828125f, -1.8237250370370373f, -1.8192752962962961f, -1.814776f, -1.8102273703703702f, -1.80562962962963f, -1.800983f, -1.7962877037037037f, -1.791543962962963f, -1.786752f, -1.7819120370370372f, -1.7770242962962963f, -1.7720889999999998f, -1.7671063703703704f, -1.7620766296296295f, -1.7570000000000001f, -1.7518767037037035f, -1.746706962962963f, -1.741491f, -1.7362290370370372f, -1.7309212962962963f, -1.725568f, -1.7201693703703702f, -1.7147256296296296f, -1.709237f, -1.7037037037037037f, -1.698125962962963f, -1.692504f, -1.6868380370370368f, -1.6811282962962961f, -1.6753749999999998f, -1.6695783703703704f, -1.6637386296296297f, -1.6578559999999998f, -1.6519307037037034f, -1.645962962962963f, -1.639953f, -1.6339010370370368f, -1.6278072962962964f, -1.621672f, -1.6154953703703703f, -1.6092776296296294f, -1.603019f, -1.5967197037037035f, -1.5903799629629627f, -1.584f, -1.5775800370370372f, -1.571120296296296f, -1.5646209999999998f, -1.5580823703703706f, -1.5515046296296293f, -1.544888f, -1.5382327037037036f, -1.531538962962963f, -1.524807f, -1.518037037037037f, -1.511229296296296f, -1.504384f, -1.4975013703703703f, -1.4905816296296295f, -1.4836249999999997f, -1.4766317037037036f, -1.4696019629629626f, -1.462536f, -1.4554340370370369f, -1.4482962962962964f, -1.4411230000000002f, -1.4339143703703703f, -1.4266706296296296f, -1.4193920000000002f, -1.4120787037037035f, -1.404730962962963f, -1.397349f, -1.3899330370370369f, -1.3824832962962963f, -1.375f, -1.3674833703703704f, -1.3599336296296296f, -1.352351f, -1.3447357037037038f, -1.3370879629629628f, -1.329408f, -1.3216960370370368f, -1.3139522962962962f, -1.306177f, -1.2983703703703702f, -1.2905326296296296f, -1.282664f, -1.2747647037037038f, -1.266834962962963f, -1.2588750000000002f, -1.250885037037037f, -1.2428652962962963f, -1.234816f, -1.2267373703703703f, -1.2186296296296297f, -1.210493f, -1.2023277037037037f, -1.194133962962963f, -1.185912f, -1.177662037037037f, -1.1693842962962964f, -1.161079f, -1.1527463703703702f, -1.1443866296296297f, -1.136f, -1.1275867037037035f, -1.119146962962963f, -1.110681f, -1.1021890370370369f, -1.0936712962962962f, -1.0851279999999999f, -1.0765593703703702f, -1.0679656296296296f, -1.059347f, -1.0507037037037037f, -1.042035962962963f, -1.033344f, -1.024628037037037f, -1.0158882962962963f, -1.007125f, -0.9983383703703703f, -0.9895286296296296f, -0.9806959999999998f, -0.9718407037037038f, -0.9629629629629629f, -0.9540629999999999f, -0.945141037037037f, -0.9361972962962962f, -0.927232f, -0.9182453703703704f, -0.9092376296296296f, -0.900209f, -0.8911597037037037f, -0.8820899629629629f, -0.873f, -0.863890037037037f, -0.8547602962962964f, -0.845611f, -0.8364423703703703f, -0.8272546296296297f, -0.8180479999999999f, -0.8088227037037036f, -0.799578962962963f, -0.7903170000000002f, -0.781037037037037f, -0.7717392962962962f, -0.762424f, -0.7530913703703703f, -0.7437416296296295f, -0.734375f, -0.7249917037037037f, -0.7155919629629629f, -0.7061759999999999f, -0.696744037037037f, -0.6872962962962963f, -0.6778329999999999f, -0.6683543703703705f, -0.6588606296296297f, -0.649352f, -0.6398287037037037f, -0.6302909629629629f, -0.6207389999999999f, -0.6111730370370371f, -0.6015932962962962f, -0.5919999999999999f, -0.5823933703703703f, -0.5727736296296295f, -0.5631409999999999f, -0.5534957037037038f, -0.543837962962963f, -0.534168f, -0.524486037037037f, -0.5147922962962963f, -0.505087f, -0.49537037037037035f, -0.4856426296296296f, -0.47590399999999994f, -0.46615470370370365f, -0.4563949629629629f, -0.44662500000000005f, -0.43684503703703703f, -0.42705529629629624f, -0.41725599999999996f, -0.4074473703703704f, -0.39762962962962956f, -0.387803f, -0.3779677037037037f, -0.3681239629629629f, -0.35827200000000003f, -0.348412037037037f, -0.3385442962962963f, -0.328669f, -0.3187863703703704f, -0.3088966296296296f, -0.29899999999999993f, -0.28909670370370366f, -0.27918696296296297f, -0.26927100000000004f, -0.25934903703703704f, -0.24942129629629628f, -0.23948799999999998f, -0.22954937037037035f, -0.21960562962962965f, -0.20965699999999998f, -0.19970370370370372f, -0.18974596296296292f, -0.179784f, -0.16981803703703705f, -0.1598482962962963f, -0.14987499999999998f, -0.1398983703703704f, -0.12991862962962963f, -0.11993599999999999f, -0.10995070370370372f, -0.09996296296296298f, -0.089973f, -0.07998103703703703f, -0.0699872962962963f, -0.05999199999999999f, -0.04999537037037037f, -0.039997629629629626f, -0.029998999999999998f, -0.019999703703703704f, -0.009999962962962962f, 0.0f, 0.00999966667999946f, 0.01999733375993093f, 0.029991003238820143f, 0.03997868031116357f, 0.04995837495787998f, 0.059928103529143496f, 0.069885890316429f, 0.07982976911113136f, 0.0897577847471601f, 0.09966799462495582f, 0.10955847021442953f, 0.11942729853438588f, 0.12927258360605834f, 0.13909244787845804f, 0.14888503362331795f, 0.1586485042974989f, 0.1683810458708147f, 0.17808086811733018f, 0.18774620586828542f, 0.197375320224904f, 0.20696649972945258f, 0.21651806149302882f, 0.22602835227867094f, 0.23549574953849794f, 0.24491866240370913f, 0.25429553262639115f, 0.2636248354722033f, 0.2729050805631327f, 0.2821348126696341f, 0.2913126124515909f, 0.30043709714765415f, 0.30950692121263845f, 0.31852077690277086f, 0.32747739480870536f, 0.3363755443363322f, 0.3452140341355209f, 0.35399171247704597f, 0.3627074675780511f, 0.3713602278765077f, 0.3799489622552249f, 0.388472680216061f, 0.39693043200507755f, 0.405321308689463f, 0.4136444421871352f, 0.42189900525000795f, 0.4300842114019795f, 0.4381993148327677f, 0.4462436102487796f, 0.4542164326822591f, 0.46211715726000974f, 0.46994519893303754f, 0.477700012168498f, 0.4853810906053716f, 0.4929879666753244f, 0.5005202111902354f, 0.5079774328978962f, 0.5153592780074097f, 0.5226654296858207f, 0.5298956075275294f, 0.5370495669980353f, 0.5441270988535676f, 0.551128028538147f, 0.5580522155596244f, 0.5648995528462251f, 0.5716699660851172f, 0.5783634130445057f, 0.5849798828807289f, 0.5915193954318165f, 0.5979820004989412f, 0.6043677771171635f, 0.6106768328168442f, 0.6169093028770649f, 0.6230653495723611f, 0.6291451614140355f, 0.6351489523872873f, 0.6410769611853464f, 0.6469294504417666f, 0.65270670596199f, 0.658409035955251f, 0.6640367702678489f, 0.6695902596187708f, 0.6750698748386078f, 0.6804760061126618f, 0.6858090622290945f, 0.6910694698329306f, 0.6962576726866814f, 0.7013741309383125f, 0.7064193203972353f, 0.7113937318189625f, 0.7162978701990245f, 0.7211322540766999f, 0.7258974148490807f, 0.7305938960959438f, 0.7352222529158692f, 0.7397830512740042f, 0.7442768673618374f, 0.7487042869693087f, 0.753065904869552f, 0.7573623242165263f, 0.7615941559557649f, 0.7657620182484389f, 0.7698665359089004f, 0.7739083398558421f, 0.7778880665771849f, 0.7818063576087741f, 0.7856638590269437f, 0.7894612209549805f, 0.7931990970835009f, 0.7968781442047267f, 0.8004990217606297f, 0.8040623914048921f, 0.8075689165786144f, 0.8110192620996813f, 0.814414093765686f, 0.8177540779702877f, 0.8210398813328771f, 0.8242721703413975f, 0.8274516110081667f, 0.8305788685385285f, 0.8336546070121552f, 0.8366794890768107f, 0.8396541756543754f, 0.8425793256589296f, 0.8454555957266803f, 0.8482836399575129f, 0.8510641096679439f, 0.8537976531552436f, 0.8564849154724974f, 0.8591265382143658f, 0.8617231593133063f, 0.8642754128460111f, 0.8667839288498187f, 0.8692493331488548f, 0.8716722471896521f, 0.874053287886007f, 0.8763930674728229f, 0.8786921933686958f, 0.880951268046997f, 0.8831708889152075f, 0.8853516482022625f, 0.8874941328536653f, 0.8895989244341314f, 0.8916665990375279f, 0.8936977272038725f, 0.8956928738431645f, 0.8976525981658167f, 0.899577453619467f, 0.9014679878319467f, 0.9033247425601896f, 0.9051482536448664f, 0.9069390509705371f, 0.9086976584311126f, 0.9104245939004265f, 0.9121203692077173f, 0.9137854901178277f, 0.9154204563159324f, 0.9170257613966083f, 0.9186018928570671f, 0.9201493320943724f, 0.9216685544064713f, 0.9231600289968692f, 0.9246242189827881f, 0.926061581406646f, 0.9274725672507033f, 0.9288576214547276f, 0.9302171829365263f, 0.9315516846152082f, 0.9328615534370349f, 0.9341472104037275f, 0.935409070603099f, 0.9366475432418873f, 0.9378630316806652f, 0.9390559334707117f, 0.9402266403927275f, 0.9413755384972874f, 0.9425030081469201f, 0.9436094240597148f, 0.9446951553543538f, 0.9457605655964739f, 0.9468060128462683f, 0.9478318497072341f, 0.9488384233759847f, 0.9498260756930403f, 0.9507951431945212f, 0.9517459571646616f, 0.9526788436890776f, 0.9535941237087119f, 0.954492113074392f, 0.9553731226019355f, 0.9562374581277391f, 0.957085420564793f, 0.9579173059590634f, 0.9587334055461876f, 0.9595340058084292f, 0.960319388531845f, 0.9610898308636139f, 0.9618456053694837f, 0.9625869800912908f, 0.9633142186045134f, 0.9640275800758169f, 0.9647273193205547f, 0.9654136868601891f, 0.9660869289795985f, 0.9667472877842368f, 0.9673950012571181f, 0.9680303033155948f, 0.9686534238679029f, 0.9692645888694489f, 0.9698640203788144f, 0.9704519366134539f, 0.9710285520050662f, 0.9715940772546178f, 0.9721487193869992f, 0.9726926818052982f, 0.9732261643446705f, 0.9737493633257944f, 0.974262471607895f, 0.9747656786413226f, 0.9752591705196751f, 0.9757431300314515f, 0.976217736711226f, 0.9766831668903339f, 0.9771395937470586f, 0.9775871873563142f, 0.9780261147388136f, 0.9784565399097167f, 0.9788786239267537f, 0.9792925249378165f, 0.9796983982280145f, 0.9800963962661914f, 0.9804866687508973f, 0.9808693626558167f, 0.9812446222746463f, 0.9816125892654238f, 0.9819734026943043f, 0.9823271990787845f, 0.9826741124303737f, 0.9830142742967113f, 0.9833478138031324f, 0.9836748576936802f, 0.9839955303715675f, 0.9843099539390887f, 0.9846182482369833f, 0.9849205308832533f, 0.985216917311436f, 0.9855075208083371f, 0.9857924525512225f, 0.9860718216444758f, 0.9863457351557237f, 0.9866142981514303f, 0.9868776137319676f, 0.9871357830661627f, 0.9873889054253283f, 0.9876370782167777f, 0.9878803970168317f, 0.9881189556033193f, 0.988352845987577f, 0.9885821584459533f, 0.9888069815508204f, 0.9890274022010992f, 0.9892435056523028f, 0.9894553755461026f, 0.9896630939394229f, 0.9898667413330675f, 0.990066396699886f, 0.9902621375124827f, 0.9904540397704736f, 0.9906421780272986f, 0.9908266254165919f, 0.9910074536781176f, 0.9911847331832744f, 0.9913585329601762f, 0.9915289207183131f, 0.9916959628727987f, 0.9918597245682077f, 0.9920202697020117f, 0.9921776609476163f, 0.9923319597770051f, 0.9924832264829984f, 0.992631520201128f, 0.9927768989311369f, 0.992919419558107f, 0.9930591378732213f, 0.9931961085941655f, 0.9933303853851733f, 0.9934620208767228f, 0.9935910666848863f, 0.9937175734303412f, 0.9938415907570454f, 0.9939631673505831f, 0.9940823509561856f, 0.9941991883964316f, 0.9943137255886342f, 0.9944260075619155f, 0.9945360784739778f, 0.9946439816275737f, 0.9947497594866797f, 0.9948534536923792f, 0.9949551050784606f, 0.9950547536867305f, 0.9951524387820521f, 0.99524819886711f, 0.9953420716969065f, 0.9954340942929939f, 0.9955243029574473f, 0.9956127332865825f, 0.9956994201844217f, 0.9957843978759136f, 0.995867699919909f, 0.9959493592219002f, 0.9960294080465228f, 0.9961078780298289f, 0.9961848001913324f, 0.9962602049458319f, 0.9963341221150144f, 0.9964065809388432f, 0.9964776100867343f, 0.9965472376685257f, 0.9966154912452406f, 0.9966823978396512f, 0.9967479839466443f, 0.996812275543394f, 0.996875298099342f, 0.9969370765859934f, 0.996997635486526f, 0.9970569988052209f, 0.997115190076715f, 0.9971722323750787f, 0.9972281483227239f, 0.9972829600991421f, 0.9973366894494792f, 0.9973893576929465f, 0.997440985731074f, 0.9974915940558062f, 0.9975412027574452f, 0.9975898315324421f, 0.9976374996910418f, 0.9976842261647808f, 0.9977300295138438f, 0.9977749279342794f, 0.997818939265078f, 0.9978620809951161f, 0.9979043702699659f, 0.997945823898577f, 0.9979864583598287f, 0.9980262898089581f, 0.9980653340838644f, 0.9981036067112925f, 0.9981411229128986f, 0.9981778976111987f, 0.9982139454354036f, 0.9982492807271415f, 0.9982839175460703f, 0.9983178696753823f, 0.9983511506272034f, 0.9983837736478877f, 0.9984157517232098f, 0.9984470975834583f, 0.9984778237084296f, 0.9985079423323266f, 0.9985374654485615f, 0.9985664048144672f, 0.9985947719559167f, 0.9986225781718535f, 0.9986498345387343f, 0.9986765519148859f, 0.9987027409447773f, 0.9987284120632095f, 0.998753575499424f, 0.9987782412811312f, 0.9988024192384614f, 0.9988261190078384f, 0.9988493500357786f, 0.9988721215826155f, 0.9988944427261528f, 0.9989163223652454f, 0.998937769223312f, 0.9989587918517782f, 0.9989793986334531f, 0.9989995977858409f, 0.9990193973643866f, 0.9990388052656595f, 0.9990578292304744f, 0.9990764768469519f, 0.9990947555535189f, 0.9991126726418513f, 0.999130235259758f, 0.99914745041401f, 0.9991643249731132f, 0.999180865670028f, 0.9991970791048344f, 0.999212971747347f, 0.9992285499396772f, 0.9992438198987459f, 0.9992587877187472f, 0.9992734593735638f, 0.9992878407191353f, 0.9993019374957799f, 0.9993157553304705f, 0.999329299739067f, 0.9993425761285043f, 0.9993555897989373f, 0.9993683459458442f, 0.999380849662089f, 0.9993931059399421f, 0.9994051196730637f, 0.9994168956584453f, 0.9994284385983162f, 0.9994397531020104f, 0.9994508436877974f, 0.9994617147846778f, 0.9994723707341434f, 0.9994828157919015f, 0.9994930541295671f, 0.9995030898363213f, 0.9995129269205365f, 0.9995225693113702f, 0.9995320208603277f, 0.9995412853427935f, 0.9995503664595334f, 0.9995592678381662f, 0.9995679930346069f, 0.9995765455344817f, 0.9995849287545152f, 0.9995931460438896f, 0.9996012006855779f, 0.9996090958976507f, 0.9996168348345574f, 0.9996244205883817f, 0.9996318561900731f, 0.9996391446106534f, 0.9996462887624f, 0.9996532915000059f, 0.9996601556217168f, 0.9996668838704453f, 0.9996734789348639f, 0.9996799434504761f, 0.9996862800006663f, 0.9996924911177296f, 0.9996985792838805f, 0.9997045469322422f, 0.9997103964478165f, 0.9997161301684341f, 0.9997217503856869f, 0.9997272593458411f, 0.9997326592507327f, 0.9997379522586461f, 0.9997431404851737f, 0.9997482260040597f, 0.9997532108480275f, 0.9997580970095901f, 0.9997628864418443f, 0.9997675810592502f, 0.9997721827383951f, 0.9997766933187409f, 0.999781114603359f, 0.9997854483596491f, 0.9997896963200444f, 0.9997938601827029f, 0.9997979416121845f, 0.9998019422401158f, 0.9998058636658408f, 0.9998097074570593f, 0.9998134751504524f, 0.9998171682522957f, 0.9998207882390607f, 0.999824336558004f, 0.9998278146277448f, 0.9998312238388316f, 0.9998345655542966f, 0.9998378411102006f, 0.9998410518161654f, 0.9998441989558979f, 0.9998472837877015f, 0.9998503075449787f, 0.9998532714367244f, 0.9998561766480074f, 0.9998590243404446f, 0.9998618156526641f, 0.9998645517007605f, 0.9998672335787403f, 0.9998698623589588f, 0.9998724390925484f, 0.9998749648098385f, 0.9998774405207668f, 0.9998798672152829f, 0.9998822458637435f, 0.9998845774173003f, 0.9998868628082794f, 0.9998891029505544f, 0.9998912987399108f, 0.9998934510544042f, 0.9998955607547113f, 0.9998976286844731f, 0.9998996556706324f, 0.9999016425237643f, 0.9999035900383996f, 0.9999054989933424f, 0.9999073701519815f, 0.9999092042625951f, 0.9999110020586502f, 0.9999127642590951f, 0.9999144915686472f, 0.9999161846780742f, 0.9999178442644705f, 0.9999194709915274f, 0.9999210655097988f, 0.9999226284569604f, 0.9999241604580655f, 0.9999256621257943f, 0.9999271340606981f, 0.9999285768514409f, 0.9999299910750331f, 0.999931377297063f, 0.9999327360719227f, 0.9999340679430292f, 0.9999353734430425f, 0.9999366530940776f, 0.9999379074079138f, 0.9999391368861988f, 0.9999403420206499f, 0.9999415232932493f, 0.9999426811764384f, 0.9999438161333054f, 0.9999449286177708f, 0.9999460190747691f, 0.9999470879404261f, 0.9999481356422338f, 0.9999491625992211f, 0.9999501692221211f, 0.9999511559135357f, 0.999952123068096f, 0.9999530710726208f, 0.9999540003062705f, 0.9999549111406991f, 0.9999558039402028f, 0.9999566790618655f, 0.9999575368557014f, 0.9999583776647955f, 0.9999592018254402f, 0.9999600096672698f, 0.999960801513393f, 0.9999615776805207f, 0.9999623384790944f, 0.9999630842134086f, 0.9999638151817338f, 0.9999645316764351f, 0.9999652339840888f, 0.9999659223855979f, 0.9999665971563038f, 0.9999672585660964f, 0.9999679068795222f, 0.9999685423558899f, 0.9999691652493744f, 0.999969775809118f, 0.9999703742793306f, 0.9999709608993864f, 0.999971535903921f, 0.9999720995229238f, 0.9999726519818308f, 0.9999731935016147f, 0.9999737242988732f, 0.9999742445859151f, 0.9999747545708461f, 0.9999752544576511f, 0.9999757444462769f, 0.9999762247327108f, 0.9999766955090599f, 0.9999771569636278f, 0.9999776092809898f, 0.9999780526420664f, 0.9999784872241964f, 0.9999789132012068f, 0.9999793307434834f, 0.9999797400180382f, 0.9999801411885765f, 0.999980534415562f, 0.9999809198562816f, 0.9999812976649076f, 0.9999816679925603f, 0.9999820309873672f, 0.9999823867945232f, 0.9999827355563483f, 0.9999830774123447f, 0.9999834124992523f, 0.9999837409511033f, 0.9999840628992768f, 0.9999843784725498f, 0.9999846877971501f, 0.9999849909968058f, 0.9999852881927952f, 0.9999855795039955f, 0.9999858650469299f, 0.9999861449358144f, 0.9999864192826038f, 0.9999866881970358f, 0.9999869517866755f, 0.999987210156958f, 0.999987463411231f, 0.9999877116507956f, 0.9999879549749473f, 0.9999881934810154f, 0.9999884272644022f, 0.9999886564186208f, 0.9999888810353327f, 0.9999891012043848f, 0.9999893170138446f, 0.999989528550036f, 0.9999897358975736f, 0.9999899391393965f, 0.9999901383568018f, 0.9999903336294768f, 0.9999905250355305f, 0.9999907126515263f, 0.9999908965525104f, 0.9999910768120439f, 0.9999912535022311f, 0.9999914266937484f, 0.999991596455873f, 0.9999917628565104f, 0.9999919259622211f, 0.9999920858382483f, 0.9999922425485428f, 0.9999923961557894f, 0.9999925467214317f, 0.9999926943056964f, 0.999992838967618f, 0.999992980765062f, 0.999993119754748f, 0.9999932559922726f, 0.9999933895321315f, 0.9999935204277413f, 0.9999936487314609f, 0.9999937744946127f, 0.9999938977675026f, 0.9999940185994405f, 0.9999941370387598f, 0.9999942531328372f, 0.9999943669281108f, 0.9999944784700996f, 0.9999945878034211f, 0.9999946949718095f, 0.9999948000181327f, 0.99999490298441f, 0.9999950039118287f, 0.9999951028407605f, 0.9999951998107778f, 0.9999952948606692f, 0.9999953880284553f, 0.9999954793514042f, 0.9999955688660456f, 0.9999956566081861f, 0.9999957426129232f, 0.9999958269146597f, 0.9999959095471169f, 0.9999959905433483f, 0.9999960699357534f, 0.9999961477560896f, 0.9999962240354857f, 0.9999962988044543f, 0.9999963720929035f, 0.9999964439301494f, 0.9999965143449275f, 0.9999965833654044f, 0.999996651019189f, 0.9999967173333433f, 0.9999967823343936f, 0.9999968460483409f, 0.9999969085006716f, 0.999996969716367f, 0.9999970297199141f, 0.9999970885353149f, 0.9999971461860959f, 0.9999972026953183f, 0.9999972580855863f, 0.9999973123790564f, 0.9999973655974467f, 0.9999974177620449f, 0.9999974688937175f, 0.9999975190129178f, 0.9999975681396938f, 0.9999976162936968f, 0.9999976634941888f, 0.9999977097600508f, 0.9999977551097893f, 0.9999977995615449f, 0.9999978431330986f, 0.9999978858418795f, 0.9999979277049719f, 0.9999979687391211f, 0.9999980089607412f, 0.9999980483859215f, 0.9999980870304326f, 0.9999981249097324f, 0.9999981620389733f, 0.9999981984330072f, 0.9999982341063923f, 0.9999982690733983f, 0.9999983033480123f, 0.9999983369439447f
};