/*
 * ASoC Driver for 
 *
 * Author:	Milan Neskovic
 *		Copyright 2016
 *		based on code by Daniel Matuschek <info@crazy-audio.com>
 *		based on code by Florian Meier <florian.meier@koalo.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>



static int snd_rpi_chipdip_dac_init(struct snd_soc_pcm_runtime *rtd)
{	
	return 0;
}

static int snd_rpi_chipdip_dac_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	int ret = 0;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	//int channels = params_channels(params);
	//int width = snd_pcm_format_physical_width(params_format(params));
        //int sample_rate = params_rate(params);
	
	ret = snd_soc_dai_set_bclk_ratio(asoc_rtd_to_cpu(rtd, 0), 2 * 32);
	
	return ret;
}

static int snd_rpi_chipdip_dac_startup(struct snd_pcm_substream *substream)
{	
	return 0;
}

static void snd_rpi_chipdip_dac_shutdown(struct snd_pcm_substream *substream)
{
	
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_chipdip_dac_ops = {
	.hw_params = snd_rpi_chipdip_dac_hw_params,
        .startup = snd_rpi_chipdip_dac_startup,
	.shutdown = snd_rpi_chipdip_dac_shutdown,
};

SND_SOC_DAILINK_DEFS(hifi,
	DAILINK_COMP_ARRAY(COMP_CPU("bcm2708-i2s.0")),
	DAILINK_COMP_ARRAY(COMP_CODEC("spdif-transmitter", "dit-hifi")),
	DAILINK_COMP_ARRAY(COMP_PLATFORM("bcm2708-i2s.0")));

static struct snd_soc_dai_link snd_rpi_chipdip_dac_dai[] = {
{
	.name		= "ChipDip DAC",
	.stream_name	= "ChipDip DAC HiFi",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
	.ops		= &snd_rpi_chipdip_dac_ops,
	.init		= snd_rpi_chipdip_dac_init,
	SND_SOC_DAILINK_REG(hifi),
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_chipdip_dac = {
	.name         = "snd_rpi_chipdip_dac",
	.driver_name  = "ChipDipDac",
	.owner        = THIS_MODULE,
	.dai_link     = snd_rpi_chipdip_dac_dai,
	.num_links    = ARRAY_SIZE(snd_rpi_chipdip_dac_dai),
};

static int snd_rpi_chipdip_dac_probe(struct platform_device *pdev)
{
	int ret = 0;

	snd_rpi_chipdip_dac.dev = &pdev->dev;

	if (pdev->dev.of_node) {
	    struct device_node *i2s_node;
	    struct snd_soc_dai_link *dai = &snd_rpi_chipdip_dac_dai[0];
	    i2s_node = of_parse_phandle(pdev->dev.of_node,
					"i2s-controller", 0);

	    if (i2s_node) {
			dai->cpus->dai_name = NULL;
			dai->cpus->of_node = i2s_node;
			dai->platforms->name = NULL;
			dai->platforms->of_node = i2s_node;
	    }
	}

	ret = devm_snd_soc_register_card(&pdev->dev, &snd_rpi_chipdip_dac);
	if (ret && ret != -EPROBE_DEFER)
		dev_err(&pdev->dev,
			"snd_soc_register_card() failed: %d\n", ret);

	pr_err("%s -> %d\n", __func__, ret);
	return ret;
}

static const struct of_device_id snd_rpi_chipdip_dac_of_match[] = {
	{ .compatible = "chipdip,chipdip-dac", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_chipdip_dac_of_match);

static struct platform_driver snd_rpi_chipdip_dac_driver = {
	.driver = {
		.name   = "snd-rpi-chipdip-dac",
		.owner  = THIS_MODULE,
		.of_match_table = snd_rpi_chipdip_dac_of_match,
	},
	.probe          = snd_rpi_chipdip_dac_probe,
};

module_platform_driver(snd_rpi_chipdip_dac_driver);

MODULE_AUTHOR("ChipDip");
MODULE_DESCRIPTION("ASoC Driver for ChipDip Sound Card");
MODULE_LICENSE("GPL v2");
