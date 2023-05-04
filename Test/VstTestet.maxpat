{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 5,
			"revision" : 4,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 77.0, 176.0, 1165.0, 794.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 2,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "TemplateOutputGridsnap",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 195.0, 162.0, 39.0, 22.0 ],
					"text" : "dbtoa"
				}

			}
, 			{
				"box" : 				{
					"annotation" : "",
					"id" : "obj-16",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 195.0, 105.0, 44.0, 48.0 ],
					"prototypename" : "gain",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_initial" : [ 0 ],
							"parameter_initial_enable" : 1,
							"parameter_linknames" : 1,
							"parameter_longname" : "live.dial[1]",
							"parameter_mmax" : 30.0,
							"parameter_mmin" : -70.0,
							"parameter_shortname" : "Gain",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.dial[1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 143.0, 192.0, 34.0, 22.0 ],
					"text" : "*~ 1."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 405.0, 180.0, 131.0, 22.0 ],
					"text" : "plug_vst3 AmpModeler"
				}

			}
, 			{
				"box" : 				{
					"basictuning" : 440,
					"clipheight" : 36.0,
					"data" : 					{
						"clips" : [ 							{
								"absolutepath" : "SulphurAeon.wav",
								"filename" : "SulphurAeon.wav",
								"filekind" : "audiofile",
								"id" : "u062000368",
								"selection" : [ 0.0, 1.0 ],
								"loop" : 1,
								"content_state" : 								{
									"loop" : 1
								}

							}
, 							{
								"absolutepath" : "Metallica.wav",
								"filename" : "Metallica.wav",
								"filekind" : "audiofile",
								"id" : "u402000367",
								"selection" : [ 0.0, 1.0 ],
								"loop" : 1,
								"content_state" : 								{
									"loop" : 1
								}

							}
 ]
					}
,
					"followglobaltempo" : 0,
					"formantcorrection" : 0,
					"id" : "obj-1",
					"maxclass" : "playlist~",
					"mode" : "basic",
					"numinlets" : 1,
					"numoutlets" : 5,
					"originallength" : [ 0.0, "ticks" ],
					"originaltempo" : 120.0,
					"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 105.0, 15.0, 255.0, 74.0 ],
					"pitchcorrection" : 0,
					"quality" : "basic",
					"timestretch" : [ 0 ]
				}

			}
, 			{
				"box" : 				{
					"annotation" : "",
					"id" : "obj-29",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 450.0, 60.0, 44.0, 48.0 ],
					"prototypename" : "freq",
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_exponent" : 3.333333,
							"parameter_initial" : [ 0 ],
							"parameter_initial_enable" : 1,
							"parameter_linknames" : 1,
							"parameter_longname" : "live.dial",
							"parameter_mmax" : 10000.0,
							"parameter_shortname" : "Freq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3
						}

					}
,
					"varname" : "live.dial"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-28",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 450.0, 120.0, 43.0, 22.0 ],
					"text" : "cycle~"
				}

			}
, 			{
				"box" : 				{
					"automatic" : 1,
					"id" : "obj-27",
					"maxclass" : "scope~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 465.0, 600.0, 315.0, 150.0 ]
				}

			}
, 			{
				"box" : 				{
					"autosave" : 1,
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 8,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "", "list", "int", "", "", "" ],
					"patching_rect" : [ 694.0, 68.0, 300.0, 100.0 ],
					"save" : [ "#N", "vst~", "loaduniqueid", 0, "C74_VST3:/Archetype Gojira", ";" ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_invisible" : 1,
							"parameter_longname" : "vst~[1]",
							"parameter_shortname" : "vst~[1]",
							"parameter_type" : 3
						}

					}
,
					"saved_object_attributes" : 					{
						"parameter_enable" : 1,
						"parameter_mappable" : 0
					}
,
					"snapshot" : 					{
						"filetype" : "C74Snapshot",
						"version" : 2,
						"minorversion" : 0,
						"name" : "snapshotlist",
						"origin" : "vst~",
						"type" : "list",
						"subtype" : "Undefined",
						"embed" : 1,
						"snapshot" : 						{
							"pluginname" : "Archetype Gojira.vst3info",
							"plugindisplayname" : "Archetype Gojira",
							"pluginsavedname" : "C74_VST3:/Archetype Gojira",
							"pluginsaveduniqueid" : 587301235,
							"version" : 1,
							"isbank" : 0,
							"isbase64" : 1,
							"sliderorder" : [  ],
							"slidervisibility" : [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
							"blob" : "7880.VMjLg7qG...OVMEUy.Ea0cVZtMEcgQWY9vSRC8Vav8lak4Fc9TCN0PiKzUkUigWQFEVYQEiX1gCLYU2aVoEdEYjKAYFTYo1ZFMVcIICUu81UY4BQPETPD4hKt3hYhUWUGM1azDSVtPDTAEjKt3hKt3xXqAiQhUWPP4RRP4hKt3hKt3hKlckKA0lXq0zUYoGMTg0bUYDU3giQh4BQPITQPQUVrUjUiIWTG4hcIcUV4UkQiAUQFMlaAslX0EzQtDjYSEzPuMzUPkjLg0VRWg0bQQEV5UjQW4TUVMFdEYTXlAkdTA0cUAEdMYjVqE0UjYWUFgzQ3vlVukzUXIVTTkEaEY0XxEUaKMCLFElKtzlXq0zUYoWUUIEQAslX0EzQtDjZlEjS2blQxEUXTgENBIFdUEiXqEUaUsVRxH1a3vVXPkjLgYWPP4xQTAESz4RZKYmK1I1Z2YUVoE0UYoVSUkUZQckV0QiQtDDUP4hPt3hKtPjKjAUQpQUPvPjKAgDTZoVPP4RRTAEVyEzQUQSPWkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt.kKtTETRUDUS4BQl4xaQYjKAYGTAcFLFIVdEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKD4xPEwjUXgVQSAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hK1sxPt.UQpQUPvPjKAgDTZoVPP4hSTYGVmkjULQzZwHldEwVXoUkQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTYGVmkjULwTUrM1Z2YjKwTjQgASUF4RPp4RPt3hKt3hKt3hatvjKPUjZTEDLD4RPHAkVpEDTtrDU1g0YIYESMsVLXkTRE4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKIQkcXcVRVwDTEwVXtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDjbPETZEwFV24RQZcVSWkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAMCTAkVQrg0ct.SX4slQi8FNrElKXcEVxU0UY4BQPIDQt3hKt3hdLkGS37jKtTETRUDUS4BQl4xaQYjKAYGTAkVQrgEdDoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDY1PEwjUXgVRCE0aMczXmQSLXsVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDYmPEwjUXgVRCM0ZYcUVxEjYic1cVM1ZAAkKIAkKt3hKt3VVrkjStLjKTETRUAUSAAkKBolQY4BQ1ITQLYEVnkzTS8VSVIkTAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKA4BUAkTUP0TPP4hPpYTVtPDTBUDSVgEZIMDUmQiQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTYGVmkDaLA0YVgUdUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1g0YIwFSPgSLh8VTWoUczXjKwTjQgASUF4RPp4RPt3hKt3BREQmUqLjKPUjZTEDLD4RPHAkVpEDTtrDU1g0YIYEToE0UZESUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKNQkcXcVRVA0bAczTuQSLZsVTF4RLEYTXvTkQtDjZtDjKt3hKt3hKtX2JC4BTEoFUAACQtDDRPokZAAkKIQkcXcVRFUEMAcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3BTt3RUPIUQTMkKDYlKuEkQtDzLPETZ2YUVmQiUPMWPsA0YMIiXtf0UXIWUWkkKDAkPD4hKt3hKt3hKtjySt3RUPIUQTMkKDYlKuEkQtDjKQETZ2YUVmQiUPMWPsAEdqESVtE0QtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4RSTYGVxUkUXQWQTElcvPkVpEjYic1cVM1ZAAkKIAkKt3hKt3BazkzLq3hKTETRUAUSAAkKBolQY4BQtPTQLYTXqUDagEDLFI1SUczX1U0Qi4BVWgkbUcUVtPDTBQjKt3hKtXGS4wjK14hKUAkTEQ0TtPjYt7VTF4RPtDUPocmUYcFMVA0bAcTU3UEaXIWUF4RLEYTXvTkQtDjZtDjKt3hKtblKw41JC4BTEoFUAACQtDDRPokZAAkKPQkcXIWUVgEcEQUX1kELgIWUWE1ZAY1XmcmUisVPP4RRP4hKt3hKtQzPBQyJt3BUAkTUP0TPP4hPpYTVtPjcCUDSFE1ZEwVXEUzUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hcqLjKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZEMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZIMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZMMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZQMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZUMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZYMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZiMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZmMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZqMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtj0Z2YEVzTjdXo2ZrM1ZAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUDTVkkbEYEYDkzUjcUUFMlKXcEVxU0UY4BQPIDQt3hKt3BYSg1Y37jKtTETRUDUS4BQl4xaQYjKAcCTAoVUFE1Yq0VTqUkQYgVQwfUbAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPjKDUDTVkkbEYEYMgiQYETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDzMPEjZUYTXms1QT8FMwjET3vVXsEjYic1cVM1ZAAkKIAkKt3hKt3hKt3xMq3hKTETRUAUSAAkKBolQY4BQPQTQPYUVxUjUjM0ZsEVZEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEAkUYIWQVQ1Tq0VXoQidgoWUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3lKD4BTEoFUAACQtDDRPokZAAkKOQkKYs1cVgEMQUEV1EUUYMWPxDlKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAQUTAoVUFE1YqcTUmEzUYMUQFMFLIcEV5sVLgQWPlM1Y2Y0XqEDTtjDTt3hKt3xLMkjQTshKtPUPIUETMEDTtHjZFkkKD4xPEAkUYIWQVQFUUYUX1giQtDSQFEFLUYjKAolKA4hKt3hKt3hKjEDQt.UQpQUPvPjKAgDTZoVPP4xRT4RVqcmUXQSTvDFcUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hYqLjKPUjZTEDLD4RPHAkVpEDTtnDUPk0cEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDYlPEgkQjETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtrxSt3RUPIUQTMkKDYlKuEkQtDzMPETaEYzXqEUQZgWUwHla3XTXpEjYic1cVM1ZAAkKIAkKt3hKt3hKtX1TtLjKTETRUAUSAAkKBolQY4BQ1ITQpwVX1U0QicTQVoEcAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUjcVk0YQYETyETaPcVSxHlKXcEVxU0UY4BQPIDQt3hKt3BRYYyM37jKtTETRUDUS4BQl4xaQYjKAoGTAIWUVgkZEQUX1MFUX8FMF4RLEYTXvTkQtDjZtDjKt3hKtDSZB01JC4BTEoFUAACQtDDRPokZAAkKLQkKgsVQFkUPvXjXMslQY4BVWgkbUcUVtPDTBQjKt3hKt.mcvLUNO4hKUAkTEQ0TtPjYt7VTF4RP2.UPxUkUXoVQTElc3P0X5EzUioWPlM1Y2Y0XqEDTtjDTt3hKt3hYYwVVt3xPtPUPIUETMEDTtHjZFkkKDAEQEYmUYcVTVA0bAcDU3UULhsFMwf0ZAY1XmcmUisVPP4RRP4hKt3hKPM1TnQyJt3BUAkTUP0TPP4hPpYTVtPjYDUjcVk0YQYETyETaTsVSxDFcEwVXoUkQtDSQFEFLUYjKAolKA4hKt3hKt3hKlsxPt.UQpQUPvPjKAgDTZoVPP4xST4RXqUjQYEDLFIFUIcUVncmUY4BVWgkbUcUVtPDTBQjKt3hKtfDQXUUNO4hKUAkTEQ0TtPjYt7VTF4RP2.UPxUkUXoVQTElcYASXxU0UgsVPlM1Y2Y0XqEDTtjDTt3hKt3BVxnFVzrhKtPUPIUETMEDTtHjZFkkKDY1PEYmUYcVTVE0cEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDA0PEYmUYcVTVE0cIQEVzEkUL4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP5AUPxUkUXoVUTIlPEwVXpkzPtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4RST4RXqUjQYUTQsA0YzXTV44hYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQPMTQ1YUVmEkUQcWRTgEcQYTStf0UXIWUWkkKDAkPD4hKt3hKtvVVrgSct3RUPIUQTMkKDYlKuEkQtDjdPEjbUYEVpUEUhITQrElZUMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtD1ZEYTVEUTaPcFMFkULtX1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUjcVk0YQYUT2kDUXQWTwzjKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAoGTAIWUVgkZUQkXBUDago1YC4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKMQkKgsVQFkUQE0FTmQiQYQiKlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEcSLXoWQrM1ZIcEToE0UZESUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKTQkcgkVTWgULUwlXDsFahsVSFMFSUw1XqcmQtDSQFEFLUYjKAolKA4hKt3hK4LUSnsxPt.UQpQUPvPjKAgDTZoVPP4hTTYWXoE0UXESUrI1SMYzX2YGUYESUFElKXcEVxU0UY4BQPIDQt3hKt3BUnETP47jKtTETRUDUS4BQl4xaQYjKAgTTAUWSFM1YYcUV3gidXoWRCM0ZYcUVxEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQ2X0X5EzUio2XTg0azXjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DUtH1ZQYEVxkzTPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtH1ZQYEVxkzPSsVVWkkbAY1XmcmUisVPP4RRP4hKt3hKynFVPMyJt3BUAkTUP0TPP4hPpYTVtPDTDUjKWkkZEYTX3ciZisVRGkEdqw1XqEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQtbUVpUjQggGTvDFcUYjKwTjQgASUF4RPp4RPt3hKtXVa0UFaqLjKPUjZTEDLD4RPHAkVpEDTt3DUtH1ZQYEVx0zTPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtHEUtH1ZQYEVx0zPQ8VSGMVcIczXugCag4BVWgkbUcUVtPDTBQjKt3hKtH2cKMFNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cwvjQqYTX5UEah4BVWgkbUcUVtPDTBQjKt3hKtH1MRISNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cwvjU3XTXv.iUY4BVWgkbUcUVtPDTBQjKt3hKtnlPW8TNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cF0TPMYzXuk0UY4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP2.UP1UkQYc1cF0DSYESXRUjQisVPlM1Y2Y0XqEDTtjDTt3hKt3hcvjlPxrhKtPUPIUETMEDTtHjZFkkKDY1PE4xUYoVQFEFLDoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDA0PE4xUYoVQFEFLPQUV1E0QZ4BVWgkbUcUVtPDTBQjKt3hKt3RUoICNO4hKUAkTEQ0TtPjYt7VTF4RPtDUP1UkQYc1cV0jQUYUVpkjUXk1bF4RLEYTXvTkQtDjZtDjKt3hKt3TRFQ0JC4BTEoFUAACQtDDRPokZAAkKKQkKhsVTVgkbUM0Tuc1QtDSQFEFLUYjKAolKA4hKt3hYjQ0LJsxPt.UQpQUPvPjKAgDTZoVPP4BST4hXqEkUXIWUoQ0YQcUVtf0UXIWUWkkKDAkPD4hKt3hKTMSMQgySt3RUPIUQTMkKDYlKuEkQtDzLPEjcUYTVmcWLhETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtrxSt3RUPIUQTMkKDYlKuEkQtDjdPEjcqYzXoclUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hcqLjKPUjZTEDLD4RPHAkVpEDTt3DUlI1ZYcUV3kjUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DUlI1ZYcUV3kjQQg2ZxT0ZQcjKwTjQgASUF4RPp4RPt3hKtX1R3ESVqLjKPUjZTEDLD4RPHAkVpEDTt7DUlI1ZYcUV3kjQR81XFo0PUczXtf0UXIWUWkkKDAkPD4hKt3hKt3hK3rDTt3RUPIUQTMkKDYlKuEkQtDzLPEDdUw1XqkTaXwDNwL1PUczXtf0UXIWUWkkKDAkPD4hKt3hKt3hKpUDTt3RUPIUQTMkKDYlKuEkQtDjcPEDdUw1XqkTaX0DNFk0ZAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPjKCUDRWkULUwlXnEUUZMWUF4RLEYTXvTkQtDjZtDjKt3hKt3hKPUmPD4BTEoFUAACQtDDRPokZAAkKOQkYh41ZGMlavXETyETaPcVSxHlKXcEVxU0UY4BQPIDQt3hKt3hKt3hK47jKtTETRUDUS4BQl4xaQYjKAcCTAg2YVQldmYUXAAiQhcTQVoEcAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPjYCUDRGoEMQcjVyUDUgYGLTokZAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPDTDUDRGoEMQcjVyUDUgYGNTMldAc0X5EjYic1cVM1ZAAkKIAkKt3hKt3hKt3RPtDjKTETRUAUSAAkKBolQY4BQ1QTQHcjVzD0QZMWQTElcAslXq0zUYQWSVkkKXcEVxU0UY4BQPIDQt3hKt3BVOozM37jKtTETRUDUS4BQl4xaQYjKAAUTAg2YVQldmYUXAAiQhIUUwHVczXEVz0jUY4BVWgkbUcUVtPDTBQjKt3hKt3hKt3RNO4hKUAkTEQ0TtPjYt7VTF4RPDEUP3clUjo2YVEVPvXjXTkzUYg1cVkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hK47jKtTETRUDUS4BQl4xaQYjKAQTTAg2YVQldmYUXAAiQhYENFEFLvXUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtjySt3RUPIUQTMkKDYlKuEkQtDjKQEDdmYEY5clUgUTQWAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hK1sxPt.UQpQUPvPjKAgDTZoVPP4xSTYlXts1Qi4FLVE0cIQEVzEkUL4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP2.UP3clUjo2YVEVQE0FTmQiQYgmKlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEgzQZQSTGo0bUQkXBUDagoVSC4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKOQkYh41ZGMlavXUT2kDUXQWTF0jKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAcCTAg2YVQldmYUXEUTaPcFMFkELtX1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPjcCUDRGoEMQcjVyUEUhITQrElZYMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt7DUlIlaqczXtAiUQcWRTgEcQESStf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDzMPEDdmYEY5clUgUTQsA0YzXTVy3hYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQ1MTQHcjVzD0QZMWUTIlPEwVXps1PtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4hSTY2XtUjUgM2ZWAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTY2XtUjUgM2ZWM0amcjKwTjQgASUF4RPp4RPt3hKt3hKt3hYqLjKPUjZTEDLD4RPHAkVpEDTt.EU1MlaEYUXys1QTUWSWokdqESXzEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQhcjVmAiUgQSTUQlcUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKtPjKyslQY8FNUE1YAcjXuQSLYkWPP4xPtbTXvLlUZQGNEMFMAcUVtPjcDUzLVkELIcEVxgSQYkWPxbUa3vlVukzUX4hKGEFLiYkVzgyZgcFLVkkKDYFQEQjZhk1YVkkdqcjXqEDdQU2aVoEdEYjK1cmUi01ZrEVYYcUV30zUZUGMF4RPhAUP2MiPLQmKC4hKLYEVn0zUZMWSG4hKDYmKoUDaXk2ZVElKDYmKuQiQYs1YG4RPTAkKt3hKt3BVVokbUYDUmE0QZcmKP4hPT4hKrslQgsVPUgkdmwFStPjYtTjKt3RZEwFV4slUg4BQ14xazXTVqc1QtDDUP4RPt3hKtfkUZIWUFQ0YQcjV24BTtHDUt3BaqYTXqETUXo2YrwjKDYlKE4hKtjVQrgUdqYUXtPjct7FMFk0ZmcjKAQETtHjKt3hKXYkVxUkQTcVTGo0ct.kKBQkKtv1ZFE1ZAUEV5cFaL4BQl4RQt3hKt3hKt3hKt3hKt3hKt3hKt3hKt3hKtQUUCUEQTg2ZrM1YQcUVDUjQicVP77RRC8Vav8lak4Fc9vyKVMEUy.Ea0cVZtMEcgQWY9.."
						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 0,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "Archetype Gojira",
									"origin" : "Archetype Gojira.vst3info",
									"type" : "VST3",
									"subtype" : "MidiEffect",
									"embed" : 0,
									"snapshot" : 									{
										"pluginname" : "Archetype Gojira.vst3info",
										"plugindisplayname" : "Archetype Gojira",
										"pluginsavedname" : "C74_VST3:/Archetype Gojira",
										"pluginsaveduniqueid" : 587301235,
										"version" : 1,
										"isbank" : 0,
										"isbase64" : 1,
										"sliderorder" : [  ],
										"slidervisibility" : [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
										"blob" : "7880.VMjLg7qG...OVMEUy.Ea0cVZtMEcgQWY9vSRC8Vav8lak4Fc9TCN0PiKzUkUigWQFEVYQEiX1gCLYU2aVoEdEYjKAYFTYo1ZFMVcIICUu81UY4BQPETPD4hKt3hYhUWUGM1azDSVtPDTAEjKt3hKt3xXqAiQhUWPP4RRP4hKt3hKt3hKlckKA0lXq0zUYoGMTg0bUYDU3giQh4BQPITQPQUVrUjUiIWTG4hcIcUV4UkQiAUQFMlaAslX0EzQtDjYSEzPuMzUPkjLg0VRWg0bQQEV5UjQW4TUVMFdEYTXlAkdTA0cUAEdMYjVqE0UjYWUFgzQ3vlVukzUXIVTTkEaEY0XxEUaKMCLFElKtzlXq0zUYoWUUIEQAslX0EzQtDjZlEjS2blQxEUXTgENBIFdUEiXqEUaUsVRxH1a3vVXPkjLgYWPP4xQTAESz4RZKYmK1I1Z2YUVoE0UYoVSUkUZQckV0QiQtDDUP4hPt3hKtPjKjAUQpQUPvPjKAgDTZoVPP4RRTAEVyEzQUQSPWkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt.kKtTETRUDUS4BQl4xaQYjKAYGTAcFLFIVdEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKD4xPEwjUXgVQSAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hK1sxPt.UQpQUPvPjKAgDTZoVPP4hSTYGVmkjULQzZwHldEwVXoUkQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTYGVmkjULwTUrM1Z2YjKwTjQgASUF4RPp4RPt3hKt3hKt3hatvjKPUjZTEDLD4RPHAkVpEDTtrDU1g0YIYESMsVLXkTRE4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKIQkcXcVRVwDTEwVXtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDjbPETZEwFV24RQZcVSWkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAMCTAkVQrg0ct.SX4slQi8FNrElKXcEVxU0UY4BQPIDQt3hKt3hdLkGS37jKtTETRUDUS4BQl4xaQYjKAYGTAkVQrgEdDoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDY1PEwjUXgVRCE0aMczXmQSLXsVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDYmPEwjUXgVRCM0ZYcUVxEjYic1cVM1ZAAkKIAkKt3hKt3VVrkjStLjKTETRUAUSAAkKBolQY4BQ1ITQLYEVnkzTS8VSVIkTAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKA4BUAkTUP0TPP4hPpYTVtPDTBUDSVgEZIMDUmQiQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTYGVmkDaLA0YVgUdUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1g0YIwFSPgSLh8VTWoUczXjKwTjQgASUF4RPp4RPt3hKt3BREQmUqLjKPUjZTEDLD4RPHAkVpEDTtrDU1g0YIYEToE0UZESUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKNQkcXcVRVA0bAczTuQSLZsVTF4RLEYTXvTkQtDjZtDjKt3hKt3hKtX2JC4BTEoFUAACQtDDRPokZAAkKIQkcXcVRFUEMAcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3BTt3RUPIUQTMkKDYlKuEkQtDzLPETZ2YUVmQiUPMWPsA0YMIiXtf0UXIWUWkkKDAkPD4hKt3hKt3hKtjySt3RUPIUQTMkKDYlKuEkQtDjKQETZ2YUVmQiUPMWPsAEdqESVtE0QtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4RSTYGVxUkUXQWQTElcvPkVpEjYic1cVM1ZAAkKIAkKt3hKt3BazkzLq3hKTETRUAUSAAkKBolQY4BQtPTQLYTXqUDagEDLFI1SUczX1U0Qi4BVWgkbUcUVtPDTBQjKt3hKtXGS4wjK14hKUAkTEQ0TtPjYt7VTF4RPtDUPocmUYcFMVA0bAcTU3UEaXIWUF4RLEYTXvTkQtDjZtDjKt3hKtblKw41JC4BTEoFUAACQtDDRPokZAAkKPQkcXIWUVgEcEQUX1kELgIWUWE1ZAY1XmcmUisVPP4RRP4hKt3hKtQzPBQyJt3BUAkTUP0TPP4hPpYTVtPjcCUDSFE1ZEwVXEUzUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hcqLjKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZEMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZIMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZMMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZQMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZUMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZYMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZiMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZmMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DU1gkbUYEVzUEUhITQrElZqMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtj0Z2YEVzTjdXo2ZrM1ZAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUDTVkkbEYEYDkzUjcUUFMlKXcEVxU0UY4BQPIDQt3hKt3BYSg1Y37jKtTETRUDUS4BQl4xaQYjKAcCTAoVUFE1Yq0VTqUkQYgVQwfUbAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPjKDUDTVkkbEYEYMgiQYETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDzMPEjZUYTXms1QT8FMwjET3vVXsEjYic1cVM1ZAAkKIAkKt3hKt3hKt3xMq3hKTETRUAUSAAkKBolQY4BQPQTQPYUVxUjUjM0ZsEVZEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEAkUYIWQVQ1Tq0VXoQidgoWUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3lKD4BTEoFUAACQtDDRPokZAAkKOQkKYs1cVgEMQUEV1EUUYMWPxDlKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAQUTAoVUFE1YqcTUmEzUYMUQFMFLIcEV5sVLgQWPlM1Y2Y0XqEDTtjDTt3hKt3xLMkjQTshKtPUPIUETMEDTtHjZFkkKD4xPEAkUYIWQVQFUUYUX1giQtDSQFEFLUYjKAolKA4hKt3hKt3hKjEDQt.UQpQUPvPjKAgDTZoVPP4xRT4RVqcmUXQSTvDFcUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hYqLjKPUjZTEDLD4RPHAkVpEDTtnDUPk0cEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDYlPEgkQjETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtrxSt3RUPIUQTMkKDYlKuEkQtDzMPETaEYzXqEUQZgWUwHla3XTXpEjYic1cVM1ZAAkKIAkKt3hKt3hKtX1TtLjKTETRUAUSAAkKBolQY4BQ1ITQpwVX1U0QicTQVoEcAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUjcVk0YQYETyETaPcVSxHlKXcEVxU0UY4BQPIDQt3hKt3BRYYyM37jKtTETRUDUS4BQl4xaQYjKAoGTAIWUVgkZEQUX1MFUX8FMF4RLEYTXvTkQtDjZtDjKt3hKtDSZB01JC4BTEoFUAACQtDDRPokZAAkKLQkKgsVQFkUPvXjXMslQY4BVWgkbUcUVtPDTBQjKt3hKt.mcvLUNO4hKUAkTEQ0TtPjYt7VTF4RP2.UPxUkUXoVQTElc3P0X5EzUioWPlM1Y2Y0XqEDTtjDTt3hKt3hYYwVVt3xPtPUPIUETMEDTtHjZFkkKDAEQEYmUYcVTVA0bAcDU3UULhsFMwf0ZAY1XmcmUisVPP4RRP4hKt3hKPM1TnQyJt3BUAkTUP0TPP4hPpYTVtPjYDUjcVk0YQYETyETaTsVSxDFcEwVXoUkQtDSQFEFLUYjKAolKA4hKt3hKt3hKlsxPt.UQpQUPvPjKAgDTZoVPP4xST4RXqUjQYEDLFIFUIcUVncmUY4BVWgkbUcUVtPDTBQjKt3hKtfDQXUUNO4hKUAkTEQ0TtPjYt7VTF4RP2.UPxUkUXoVQTElcYASXxU0UgsVPlM1Y2Y0XqEDTtjDTt3hKt3BVxnFVzrhKtPUPIUETMEDTtHjZFkkKDY1PEYmUYcVTVE0cEoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hK2rhKtPUPIUETMEDTtHjZFkkKDA0PEYmUYcVTVE0cIQEVzEkUL4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP5AUPxUkUXoVUTIlPEwVXpkzPtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4RST4RXqUjQYUTQsA0YzXTV44hYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQPMTQ1YUVmEkUQcWRTgEcQYTStf0UXIWUWkkKDAkPD4hKt3hKtvVVrgSct3RUPIUQTMkKDYlKuEkQtDjdPEjbUYEVpUEUhITQrElZUMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtD1ZEYTVEUTaPcFMFkULtX1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPDTCUjcVk0YQYUT2kDUXQWTwzjKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAoGTAIWUVgkZUQkXBUDago1YC4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKMQkKgsVQFkUQE0FTmQiQYQiKlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEcSLXoWQrM1ZIcEToE0UZESUF4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKTQkcgkVTWgULUwlXDsFahsVSFMFSUw1XqcmQtDSQFEFLUYjKAolKA4hKt3hK4LUSnsxPt.UQpQUPvPjKAgDTZoVPP4hTTYWXoE0UXESUrI1SMYzX2YGUYESUFElKXcEVxU0UY4BQPIDQt3hKt3BUnETP47jKtTETRUDUS4BQl4xaQYjKAgTTAUWSFM1YYcUV3gidXoWRCM0ZYcUVxEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQ2X0X5EzUio2XTg0azXjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DUtH1ZQYEVxkzTPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtzDUtH1ZQYEVxkzPSsVVWkkbAY1XmcmUisVPP4RRP4hKt3hKynFVPMyJt3BUAkTUP0TPP4hPpYTVtPDTDUjKWkkZEYTX3ciZisVRGkEdqw1XqEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQtbUVpUjQggGTvDFcUYjKwTjQgASUF4RPp4RPt3hKtXVa0UFaqLjKPUjZTEDLD4RPHAkVpEDTt3DUtH1ZQYEVx0zTPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTtHEUtH1ZQYEVx0zPQ8VSGMVcIczXugCag4BVWgkbUcUVtPDTBQjKt3hKtH2cKMFNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cwvjQqYTX5UEah4BVWgkbUcUVtPDTBQjKt3hKtH1MRISNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cwvjU3XTXv.iUY4BVWgkbUcUVtPDTBQjKt3hKtnlPW8TNO4hKUAkTEQ0TtPjYt7VTF4RPy.UP1UkQYc1cF0TPMYzXuk0UY4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP2.UP1UkQYc1cF0DSYESXRUjQisVPlM1Y2Y0XqEDTtjDTt3hKt3hcvjlPxrhKtPUPIUETMEDTtHjZFkkKDY1PE4xUYoVQFEFLDoGV5sFaisVPlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDA0PE4xUYoVQFEFLPQUV1E0QZ4BVWgkbUcUVtPDTBQjKt3hKt3RUoICNO4hKUAkTEQ0TtPjYt7VTF4RPtDUP1UkQYc1cV0jQUYUVpkjUXk1bF4RLEYTXvTkQtDjZtDjKt3hKt3TRFQ0JC4BTEoFUAACQtDDRPokZAAkKKQkKhsVTVgkbUM0Tuc1QtDSQFEFLUYjKAolKA4hKt3hYjQ0LJsxPt.UQpQUPvPjKAgDTZoVPP4BST4hXqEkUXIWUoQ0YQcUVtf0UXIWUWkkKDAkPD4hKt3hKTMSMQgySt3RUPIUQTMkKDYlKuEkQtDzLPEjcUYTVmcWLhETSFM1aYcUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtrxSt3RUPIUQTMkKDYlKuEkQtDjdPEjcqYzXoclUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hcqLjKPUjZTEDLD4RPHAkVpEDTt3DUlI1ZYcUV3kjUPkVTWoULUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt3DUlI1ZYcUV3kjQQg2ZxT0ZQcjKwTjQgASUF4RPp4RPt3hKtX1R3ESVqLjKPUjZTEDLD4RPHAkVpEDTt7DUlI1ZYcUV3kjQR81XFo0PUczXtf0UXIWUWkkKDAkPD4hKt3hKt3hK3rDTt3RUPIUQTMkKDYlKuEkQtDzLPEDdUw1XqkTaXwDNwL1PUczXtf0UXIWUWkkKDAkPD4hKt3hKt3hKpUDTt3RUPIUQTMkKDYlKuEkQtDjcPEDdUw1XqkTaX0DNFk0ZAY1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPjKCUDRWkULUwlXnEUUZMWUF4RLEYTXvTkQtDjZtDjKt3hKt3hKPUmPD4BTEoFUAACQtDDRPokZAAkKOQkYh41ZGMlavXETyETaPcVSxHlKXcEVxU0UY4BQPIDQt3hKt3hKt3hK47jKtTETRUDUS4BQl4xaQYjKAcCTAg2YVQldmYUXAAiQhcTQVoEcAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPjYCUDRGoEMQcjVyUDUgYGLTokZAY1XmcmUisVPP4RRP4hKt3hKt3hKtLyJt3BUAkTUP0TPP4hPpYTVtPDTDUDRGoEMQcjVyUDUgYGNTMldAc0X5EjYic1cVM1ZAAkKIAkKt3hKt3hKt3RPtDjKTETRUAUSAAkKBolQY4BQ1QTQHcjVzD0QZMWQTElcAslXq0zUYQWSVkkKXcEVxU0UY4BQPIDQt3hKt3BVOozM37jKtTETRUDUS4BQl4xaQYjKAAUTAg2YVQldmYUXAAiQhIUUwHVczXEVz0jUY4BVWgkbUcUVtPDTBQjKt3hKt3hKt3RNO4hKUAkTEQ0TtPjYt7VTF4RPDEUP3clUjo2YVEVPvXjXTkzUYg1cVkkKXcEVxU0UY4BQPIDQt3hKt3hKt3hK47jKtTETRUDUS4BQl4xaQYjKAQTTAg2YVQldmYUXAAiQhYENFEFLvXUVtf0UXIWUWkkKDAkPD4hKt3hKt3hKtjySt3RUPIUQTMkKDYlKuEkQtDjKQEDdmYEY5clUgUTQWAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hK1sxPt.UQpQUPvPjKAgDTZoVPP4xSTYlXts1Qi4FLVE0cIQEVzEkUL4BVWgkbUcUVtPDTBQjKt3hKt3hKt3hKt3hKUAkTEQ0TtPjYt7VTF4RP2.UP3clUjo2YVEVQE0FTmQiQYgmKlM1Y2Y0XqEDTtjDTt3hKt3hKt3hKt3hKtPUPIUETMEDTtHjZFkkKDY2PEgzQZQSTGo0bUQkXBUDagoVSC4RLEYTXvTkQtDjZtDjKt3hKt3hKt3hKt3BTEoFUAACQtDDRPokZAAkKOQkYh41ZGMlavXUT2kDUXQWTF0jKXcEVxU0UY4BQPIDQt3hKt3hKt3hKt3hKtTETRUDUS4BQl4xaQYjKAcCTAg2YVQldmYUXEUTaPcFMFkELtX1XmcmUisVPP4RRP4hKt3hKt3hKt3hKt3BUAkTUP0TPP4hPpYTVtPjcCUDRGoEMQcjVyUEUhITQrElZYMjKwTjQgASUF4RPp4RPt3hKt3hKt3hKt3hKPUjZTEDLD4RPHAkVpEDTt7DUlIlaqczXtAiUQcWRTgEcQESStf0UXIWUWkkKDAkPD4hKt3hKt3hKt3hKt3RUPIUQTMkKDYlKuEkQtDzMPEDdmYEY5clUgUTQsA0YzXTVy3hYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQ1MTQHcjVzD0QZMWUTIlPEwVXps1PtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4hSTY2XtUjUgM2ZWAUZQckVwTkQtDSQFEFLUYjKAolKA4hKt3hKt3hKt3hKt.UQpQUPvPjKAgDTZoVPP4xRTY2XtUjUgM2ZWM0amcjKwTjQgASUF4RPp4RPt3hKt3hKt3hYqLjKPUjZTEDLD4RPHAkVpEDTt.EU1MlaEYUXys1QTUWSWokdqESXzEjYic1cVM1ZAAkKIAkKt3hKt3hKt3hKt3hKTETRUAUSAAkKBolQY4BQtLTQhcjVmAiUgQSTUQlcUYjKwTjQgASUF4RPp4RPt3hKt3hKt3hKtPjKyslQY8FNUE1YAcjXuQSLYkWPP4xPtbTXvLlUZQGNEMFMAcUVtPjcDUzLVkELIcEVxgSQYkWPxbUa3vlVukzUX4hKGEFLiYkVzgyZgcFLVkkKDYFQEQjZhk1YVkkdqcjXqEDdQU2aVoEdEYjK1cmUi01ZrEVYYcUV30zUZUGMF4RPhAUP2MiPLQmKC4hKLYEVn0zUZMWSG4hKDYmKoUDaXk2ZVElKDYmKuQiQYs1YG4RPTAkKt3hKt3BVVokbUYDUmE0QZcmKP4hPT4hKrslQgsVPUgkdmwFStPjYtTjKt3RZEwFV4slUg4BQ14xazXTVqc1QtDDUP4RPt3hKtfkUZIWUFQ0YQcjV24BTtHDUt3BaqYTXqETUXo2YrwjKDYlKE4hKtjVQrgUdqYUXtPjct7FMFk0ZmcjKAQETtHjKt3hKXYkVxUkQTcVTGo0ct.kKBQkKtv1ZFE1ZAUEV5cFaL4BQl4RQt3hKt3hKt3hKt3hKt3hKt3hKt3hKt3hKtQUUCUEQTg2ZrM1YQcUVDUjQicVP77RRC8Vav8lak4Fc9vyKVMEUy.Ea0cVZtMEcgQWY9.."
									}
,
									"fileref" : 									{
										"name" : "Archetype Gojira",
										"filename" : "Archetype Gojira.maxsnap",
										"filepath" : "D:/Documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "65a639c381ac4f3c72afa876036e27dc"
									}

								}
 ]
						}

					}
,
					"text" : "vst~ \"C74_VST3:/Archetype Gojira\"",
					"varname" : "vst~[1]",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-14",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 45.0, 615.0, 48.0, 136.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.gain~[1]",
							"parameter_mmax" : 6.0,
							"parameter_mmin" : -70.0,
							"parameter_shortname" : "live.gain~",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.gain~[1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-10",
					"maxclass" : "spectroscope~",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 150.0, 465.0, 300.0, 100.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 45.0, 262.0, 48.0, 136.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.gain~",
							"parameter_mmax" : 6.0,
							"parameter_mmin" : -70.0,
							"parameter_shortname" : "live.gain~",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.gain~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "ezadc~",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 45.0, 15.0, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "mc.ezdac~",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 840.0, 360.0, 60.0, 60.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 840.0, 285.0, 60.0, 60.0 ]
				}

			}
, 			{
				"box" : 				{
					"channels" : 2,
					"id" : "obj-4",
					"lastchannelcount" : 2,
					"maxclass" : "mc.live.gain~",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "multichannelsignal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 782.5, 285.0, 48.0, 136.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "mc.live.gain~",
							"parameter_mmax" : 0.0,
							"parameter_mmin" : -70.0,
							"parameter_shortname" : "MasterVol",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "mc.live.gain~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "multichannelsignal" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 5,
							"revision" : 4,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 1061.0, 192.0, 640.0, 480.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 240.0, 143.0, 33.0, 22.0 ],
									"text" : "== 0"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-12",
									"index" : 3,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 240.0, 83.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-11",
									"index" : 2,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 173.0, 83.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-10",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 122.0, 83.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "multichannelsignal" ],
									"patching_rect" : [ 122.0, 143.0, 70.0, 22.0 ],
									"text" : "mc.pack~ 2"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-8",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 122.0, 287.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 1,
									"outlettype" : [ "multichannelsignal" ],
									"patching_rect" : [ 122.0, 219.0, 84.0, 22.0 ],
									"text" : "mc.clip~ -1. 1."
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "multichannelsignal" ],
									"patching_rect" : [ 122.0, 180.0, 53.0, 22.0 ],
									"text" : "mc.*~ 1."
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 1 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-13", 0 ],
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 1 ],
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 782.5, 255.0, 66.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p ClipMute"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"order" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"order" : 1,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 1 ],
					"source" : [ "obj-17", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-28", 0 ],
					"source" : [ "obj-29", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 2 ],
					"source" : [ "obj-6", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-14" : [ "live.gain~[1]", "live.gain~", 0 ],
			"obj-16" : [ "live.dial[1]", "Gain", 0 ],
			"obj-29" : [ "live.dial", "Freq", 0 ],
			"obj-4" : [ "mc.live.gain~", "MasterVol", 0 ],
			"obj-5" : [ "live.gain~", "live.gain~", 0 ],
			"obj-7" : [ "vst~[1]", "vst~[1]", 0 ],
			"parameterbanks" : 			{
				"0" : 				{
					"index" : 0,
					"name" : "",
					"parameters" : [ "-", "-", "-", "-", "-", "-", "-", "-" ]
				}

			}
,
			"inherited_shortname" : 1
		}
,
		"dependency_cache" : [ 			{
				"name" : "Archetype Gojira.maxsnap",
				"bootpath" : "D:/Documents/Max 8/Snapshots",
				"patcherrelativepath" : "../../../Max 8/Snapshots",
				"type" : "mx@s",
				"implicit" : 1
			}
, 			{
				"name" : "Metallica.wav",
				"bootpath" : "D:/Documents/C++/AmpModeler/Test",
				"patcherrelativepath" : ".",
				"type" : "WAVE",
				"implicit" : 1
			}
, 			{
				"name" : "SulphurAeon.wav",
				"bootpath" : "D:/Documents/C++/AmpModeler/Test",
				"patcherrelativepath" : ".",
				"type" : "WAVE",
				"implicit" : 1
			}
 ],
		"autosave" : 0,
		"boxgroups" : [ 			{
				"boxes" : [ "obj-6", "obj-8", "obj-2", "obj-4" ]
			}
 ]
	}

}
