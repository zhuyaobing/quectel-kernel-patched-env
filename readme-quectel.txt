Software environment description：
1、imx_v7_defconfig 、linux-3.14 、SDK sourced from “alex-kernel-patched-env”
2、pacth：pacths required to compile the kernel3.14
3、qcacld-2.0 ：The driver source code of version 3.14 provided by Qualcomm(QCA6574AU_LE_2_2_1_For_Linux_Kernel_3_14_Release_Notes.pdf)
   $repo init -u git://codeaurora.org/quic/le/le/manifest.git -b release -m WLANHOST_AU.FSL.3.10-00500-QCA6574AUARMSDIOHZ.xml --repo-url=git://codeaurora.org/tools/repo.git --repo-branch=caf-stable
   $repo sync
   
一、Compile method：
Please do the following before compiling:
1：Specify the kernel path
             $cd qcacld-2.0
	     $vim Makefile 
			     KERNEL_SRC ?= home/quectel-kernel-patched-env/linux-3.14

2、Specify the cross-compilation toolchain
   source /home/quectel-kernel-patched-env/SDK/environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi
   
compile:
     $cd linux-3.14
     $make ARCH=arm defconfig KBUILD_DEFCONFIG=imx_v7_defconfig
     $make ARCH=arm Image -j8
     $cd qcacld-2.0
     $make CONFGI_ARM_MDMFERMIUM=n CONFIG_WLAN_FEATURE_SAE=y WLAN_OPEN_SOURCE=1 CONFIG_WLAN_FEATURE_11W=y CONFIG_LINUX_QCMBR=y PANIC_ON_BUG=n CONFIG_CLD_HL_SDIO_CORE=y CONFIG_FRAME_WARN=4096 CONFIG_NON_QC_PLATFORM=y MODNAME=wlan CHIP_NAME=qca6574 -Wno-error


二、quectel-kernel-patched-env compilation detailed instructions:
step 1：Find linux-imx_3.14.28.bbappend in qcacld-2.0/sources/meta-qti-connectivity/recipes-kernel/linux-kernel
step 2: Modify imx_v7_defconfig according to linux-imx_3.14.28.bbappend
step 3: Download and merge the pacths(quectel-kernel-patched-env/pacth) related to linux3.14 according to linux-imx_3.14.28.bbappend
Step 4: Modify the following files in the qcacld-2.0
        1、\qcacld-2.0\CORE\HDD\inc\wlan_hdd_cfg80211.h
		  ---Comment out the following code snippet
    	 /*
          #if !(defined (SUPPORT_WDEV_CFG80211_VENDOR_EVENT_ALLOC))
          static inline struct sk_buff *
          backported_cfg80211_vendor_event_alloc(struct wiphy *wiphy,
          					struct wireless_dev *wdev,
          					int approxlen,
          					int event_idx, gfp_t gfp)
          {
          	return cfg80211_vendor_event_alloc(wiphy, approxlen, event_idx, gfp);
          }
         #define cfg80211_vendor_event_alloc backported_cfg80211_vendor_event_alloc
         #endif
    	*/
    	
    	 2、\qcacld-2.0\CORE\HDD\src\wlan_hdd_cfg80211.h
    	    ---Change sinfo to rinfo
    	 static void hdd_fill_bw_mcs(struct rate_info *rinfo,
    	 
    	 3、\qcacld-2.0\CORE\HDD\src\wlan_hdd_main.c 
         static void hdd_stop_p2p_go(hdd_adapter_t *adapter)
        {
    	  //hdd_context_t *hdd_ctx = (hdd_context_t *)adapter->pHddCtx;
    	   hddLog(VOS_TRACE_LEVEL_ERROR, "[SSR] send stop iface ap to supplicant");
    	  //cfg80211_stop_iface(hdd_ctx->wiphy, &adapter->wdev, GFP_KERNEL);
    	   cfg80211_ap_stopped(adapter->dev, GFP_KERNEL);
        }
    	 
    	3、qcacld-2.0\CORE\CLD_TXRX\HTT\htt_h2t.c
    	 int htt_h2t_chan_cali_data_msg(struct htt_pdev_t *pdev,
    			       u32 freq)
        {
    	 ....
    	 ....
    	 	if ((pdev->cfg.is_high_latency) &&
    	    (!pdev->cfg.default_tx_comp_req)) {
    		tx_credit_availability =
    		    ol_tx_target_credit_dec(pdev->txrx_pdev, 2);
    		if (tx_credit_availability == A_ERROR) {
    			//u_int64_t tx_pending_record = 0;
    
    			adf_os_print("htt dl cali pending");
    			//tx_pending_record = ktime_get_ns();
    			ol_tx_cali_pending_status_set(pdev->txrx_pdev, 1);
    			rc =
    			  wait_for_completion_timeout(&pdev->tx_cali_resource,
    						      msecs_to_jiffies(15));
    			if (!rc) {
    				//adf_os_print("dl cali pending too long %lld",
    					     //(ktime_get_ns() -
    						 //tx_pending_record));
    				ol_tx_cali_pending_status_set(pdev->txrx_pdev, 0);
    				return A_NO_RESOURCE;
    			}
    			tx_credit_availability = A_OK;
    		}
    	}
    	.....
      }
Step 5: 
     $cp imx_v7_defconfig linux-3.14/arch/arm/configs/
     $cd linux-3.14
     $make ARCH=arm defconfig KBUILD_DEFCONFIG=imx_v7_defconfig
     $make ARCH=arm Image -j8
     $cd qcacld-2.0
     $make CONFGI_ARM_MDMFERMIUM=n CONFIG_WLAN_FEATURE_SAE=y WLAN_OPEN_SOURCE=1 CONFIG_WLAN_FEATURE_11W=y CONFIG_LINUX_QCMBR=y PANIC_ON_BUG=n CONFIG_CLD_HL_SDIO_CORE=y CONFIG_FRAME_WARN=4096 CONFIG_NON_QC_PLATFORM=y MODNAME=wlan CHIP_NAME=qca6574 -Wno-error


    
   
   
	
    
	
