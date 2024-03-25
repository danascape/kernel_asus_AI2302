/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 */

// ASUS_BSP +++
#include <linux/device.h>
#include <linux/power_supply.h>
// ASUS_BSP ---

#ifndef _BATTERY_CHARGER_H
#define _BATTERY_CHARGER_H

#include <linux/notifier.h>

enum battery_charger_prop {
	BATTERY_RESISTANCE,
	BATTERY_CHARGER_PROP_MAX,
};

enum bc_hboost_event {
	VMAX_CLAMP,
};

// ASUS_BSP +++
#define QTI_POWER_SUPPLY_CHARGED   0x0001
#define QTI_POWER_SUPPLY_UNCHARGED 0x0002
extern void qti_charge_register_notify(struct notifier_block *nb);
extern void qti_charge_unregister_notify(struct notifier_block *nb);
// ASUS_BSP ---

#if IS_ENABLED(CONFIG_QTI_BATTERY_CHARGER)
int qti_battery_charger_get_prop(const char *name,
				enum battery_charger_prop prop_id, int *val);
int register_hboost_event_notifier(struct notifier_block *nb);
int unregister_hboost_event_notifier(struct notifier_block *nb);
#else
static inline int
qti_battery_charger_get_prop(const char *name,
				enum battery_charger_prop prop_id, int *val)
{
	return -EINVAL;
}

static inline int register_hboost_event_notifier(struct notifier_block *nb)
{
	return -EOPNOTSUPP;
}

static inline int unregister_hboost_event_notifier(struct notifier_block *nb)
{
	return -EOPNOTSUPP;
}
#endif

//[+++] ASUS_BSP : Add for sub-function
struct psy_state {
	struct power_supply	*psy;
	char			*model;
	const int		*map;
	u32			*prop;
	u32			prop_count;
	u32			opcode_get;
	u32			opcode_set;
};

enum psy_type {
	PSY_TYPE_BATTERY,
	PSY_TYPE_USB,
	PSY_TYPE_WLS,
	PSY_TYPE_MAX,
};

struct battery_chg_dev {
	struct device			*dev;
	struct class			battery_class;
	struct pmic_glink_client	*client;
	struct mutex			rw_lock;
	struct completion		ack;
	struct completion		fw_buf_ack;
	struct completion		fw_update_ack;
	struct psy_state		psy_list[PSY_TYPE_MAX];
	struct dentry			*debugfs_dir;
	void				*notifier_cookie;
	u32				*thermal_levels;
	const char			*wls_fw_name;
	int				curr_thermal_level;
	int				num_thermal_levels;
	int				shutdown_volt_mv;
	atomic_t			state;
	struct work_struct		subsys_up_work;
	struct work_struct		usb_type_work;
	struct work_struct		battery_check_work;
	int				fake_soc;
	bool				block_tx;
	bool				ship_mode_en;
	bool				debug_battery_detected;
	bool				wls_fw_update_reqd;
	u32				wls_fw_version;
	u16				wls_fw_crc;
	u32				wls_fw_update_time_ms;
	struct notifier_block		reboot_notifier;
	u32				thermal_fcc_ua;
	u32				restrict_fcc_ua;
	u32				last_fcc_ua;
	u32				usb_icl_ua;
	u32				thermal_fcc_step;
	bool				restrict_chg_en;
	/* To track the driver initialization status */
	bool				initialized;
	bool				notify_en;
};

enum battman_oem_property {
    BATTMAN_OEM_PM8350B_ICL,
    BATTMAN_OEM_SMB1396_ICL,
    BATTMAN_OEM_FCC,
    BATTMAN_OEM_DEBUG_MASK,
    BATTMAN_OEM_USBIN_SUSPEND,
    BATTMAN_OEM_CHARGING_SUSPNED,
    BATTMAN_OEM_Panel_Check,
    BATTMAN_OEM_Write_PM8350B_Register,
    BATTMAN_OEM_CHG_MODE,
    BATTMAN_OEM_FV,
    BATTMAN_OEM_In_Call,
    BATTMAN_OEM_WORK_EVENT,
    //ATD +++
    BATTMAN_OEM_ADSP_PLATFORM_ID,
    BATTMAN_OEM_BATT_ID,
    BATTMAN_OEM_CHG_LIMIT_EN,
    BATTMAN_OEM_CHG_LIMIT_CAP,
    //ATD ---
    BATTMAN_OEM_Batt_Protection,
    BATTMAN_OEM_DEMOAPP,
    BATTMAN_OEM_THERMAL_THRESHOLD,
    BATTMAN_OEM_SLOW_CHG,
    BATTMAN_OEM_THERMAL_ALERT,
    BATTMAN_OEM_CHG_Disable_Jeita,
    BATTMAN_OEM_FG_SoC,
    BATTMAN_OEM_WLC_IC_STATUS,
    BATTMAN_OEM_WLC_FW_UPDATE,
    BATTMAN_OEM_WLC_CEP_VALUE,
    BATTMAN_OEM_Write_NU1628_Register,
    BATTMAN_OEM_Read_NU1628_Register,
	BATTMAN_OEM_WLC_VRECT_VALUE,
  	BATTMAN_OEM_WLC_VOUT_VALUE,
  	BATTMAN_OEM_WLC_IOUT_VALUE,
  	BATTMAN_OEM_WLC_TEMP_VALUE,
  	BATTMAN_OEM_WLC_VOUT_SET_VALUE,
  	BATTMAN_OEM_WLC_OPMODE_VALUE,
  	BATTMAN_OEM_WLC_OP_FREQ_VALUE,
  	BATTMAN_OEM_WLC_PING_FREQ_VALUE,
  	BATTMAN_OEM_WLC_OVP_VALUE,
  	BATTMAN_OEM_WLC_EPP_W_VALUE,
	BATTMAN_OEM_WLC_THERMAL,
	BATTMAN_OEM_SKIN_THERMAL_AC_LIMIT,
	BATTMAN_OEM_WLC_CHARGE_TYPE,
    BATTMAN_OEM_PROPERTY_MAX,
};

enum Work_ID {
    WORK_JEITA_RULE,
    WORK_JEITA_CC,
    WORK_18W_WORKAROUND,
    WORK_PANEL_CHECK,
    WORK_LONG_FULL_CAP,
    WORK_MAX
};

enum thermal_alert_state {
    THERMAL_ALERT_NONE,
    THERMAL_ALERT_NO_AC,
    THERMAL_ALERT_WITH_AC,
	THERMAL_ALERT_MAX
};

enum phone_skin_thermal_ac_state {
	PHONE_SKIN_THERMAL_NONE,
    PHONE_SKIN_THERMAL_NO_LIMITE,
    PHONE_SKIN_THERMAL_LIMITE_1,
    PHONE_SKIN_THERMAL_LIMITE_2,
	PHONE_SKIN_THERMAL_LIMITE_3,
};
  
enum jeita_state_type {
    // Initial state
    S_JEITA_CC_INITIAL,

    S_JEITA_CC_STBY,
    S_JEITA_CC_PRE_CHG,
    S_JEITA_CC_1,
    S_JEITA_CC_2,
    S_JEITA_CV,

    // For counting
    S_JEITA_CC_STATE_MAX,
};

#define ASUS_CHARGER_TYPE_LEVEL0 0 // For disconnection, reset to default
#define ASUS_CHARGER_TYPE_LEVEL1 1 // This is for normal 18W QC3 or PD
#define ASUS_CHARGER_TYPE_LEVEL2 2 // This is for ASUS 30W adapter
#define ASUS_CHARGER_TYPE_LEVEL3 3 // This is for ASUS 65W adapter

#define SWITCH_LEVEL4_NOT_QUICK_CHARGING    8 //Now, this is used for 65W
#define SWITCH_LEVEL4_QUICK_CHARGING        7 //Now, this is used for 65W
#define SWITCH_LEVEL3_NOT_QUICK_CHARGING    6 //EQual to SWITCH_NXP_NOT_QUICK_CHARGING(ASUS 30W)
#define SWITCH_LEVEL3_QUICK_CHARGING        5 //EQual to SWITCH_NXP_QUICK_CHARGING(ASUS 30W)
#define SWITCH_LEVEL1_NOT_QUICK_CHARGING    4 //EQual to SWITCH_QC_NOT_QUICK_CHARGING(DCP 10W)
#define SWITCH_LEVEL1_QUICK_CHARGING        3 //EQual to SWITCH_QC_QUICK_CHARGING (DCP 10W)
#define SWITCH_LEVEL2_NOT_QUICK_CHARGING    2 //EQual to SWITCH_QC_NOT_QUICK_CHARGING_PLUS (QC 18W)
#define SWITCH_LEVEL2_QUICK_CHARGING        1 //EQual to SWITCH_QC_QUICK_CHARGING_PLUS (QC 18W)
#define SWITCH_LEVEL0_DEFAULT               0 //EQual to SWITCH_QC_OTHER

int asuslib_init(void);
int asuslib_deinit(void);
//int asus_chg_resume(struct device *dev);
//void set_qc_stat(int status);
//[---] ASUS_BSP : Add for sub-function
#endif
