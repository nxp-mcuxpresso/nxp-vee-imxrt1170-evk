#
# Copyright 2023, 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifeq ($(OS),Windows_NT)
S2SEXE=$(BSP_DIR)/projects/common/scripts/s2s.bat
LINK=&&
TOUCH=cmd /c type nul >
EXPORT=cmd /c SET
UNZIP=tar -xf
FIND_JAVA_FILES_IN_APP_DIR=$(shell cmd /c dir /s /b "$(subst /,\,$(APP_DIR))\*.java")
else
S2SEXE=$(BSP_DIR)/projects/common/scripts/s2s.sh
LINK=;
TOUCH=touch
EXPORT=export
UNZIP=unzip
FIND_JAVA_FILES_IN_APP_DIR=$(shell find $(APP_DIR) -name "*.java")
endif

.PRECIOUS: .java.mock .java.configuration .%.java.app

.java.mock:
	cd $(MOCK_DIR) $(LINK) $(MICROEJ_BUILDKIT_PATH_VAR)/bin/mmm publish \
		-Declipse.home="$(ECLIPSE_HOME)" \
		-Dcom.microej.platformbuilder.architecture.usage=$(USAGE) \
		$(MODULE_REPOSITORY_SETTINGS) \
		$(JAVA_VERBOSE)
	$(TOUCH) $@

.java.configuration: $(CONF_DIR)/module.ivy
	cd $(CONF_DIR) $(LINK) $(MICROEJ_BUILDKIT_PATH_VAR)/bin/mmm publish $(PUBLISH_MODE) \
		$(PUBLISH_ARTIFACTS) \
		-Declipse.home="$(ECLIPSE_HOME)" \
		-Dcom.microej.platformbuilder.architecture.usage=$(USAGE) \
		$(MODULE_REPOSITORY_SETTINGS) \
		$(JAVA_VERBOSE)
	$(TOUCH) $@

.java.test.s2s:
	$(S2SEXE) $(S2S_TTY) $(S2S_PORT) &

javasec.prop :
	$(MICROEJ_SEC)

$(addprefix .java.test.,$(VALIDATIONS)): javasec.prop
	cd $(TEST_DIR$@) $(LINK) $(EXPORT) TEST_TYPE="ARMGCC" BOARD=$(BOARD) $(LINK) $(MICROEJ_BUILDKIT_PATH_VAR)/bin/mmm publish  \
		-Declipse.home="$(ECLIPSE_HOME)" \
		$(MODULE_REPOSITORY_SETTINGS) \
		-Dcom.microej.platformbuilder.architecture.usage=$(USAGE) \
		-Dplatform-loader.target.platform.dir=$(PLAT_DIR)/source \
		-Ddeploy.bsp.root.dir=$(BSP_DIR) \
		-Dlaunch.properties.jvm="-Djava.security.properties=$(BASE_DIR)/javasec.prop" \
		-D"local.repo.url=$(BASE_DIR)/test_results/" \
		-Dplatform-launcher.platform.dir=$(PLAT_DIR)/source $(JAVA_VERBOSE)

.%.java.app: javasec.prop  $(APP_DIR)/module.ivy $(FIND_JAVA_FILES_IN_APP_DIR)
	cd $(APP_DIR) $(LINK) $(MICROEJ_BUILDKIT_PATH_VAR)/bin/mmm publish  \
		-Declipse.home="$(ECLIPSE_HOME)" \
		$(MODULE_REPOSITORY_SETTINGS) \
		-Dcom.microej.platformbuilder.architecture.usage=$(USAGE) \
		-Dplatform-loader.target.platform.dir=$(PLAT_DIR)/source \
		-Dplatform-launcher.platform.dir=$(PLAT_DIR)/source \
		-Dapplication.main.class=$(if $(MAIN),$(MAIN),$(MAIN$@)) \
		-Dlaunch.properties.jvm="-Djava.security.properties=$(BASE_DIR)/javasec.prop" \
		-Ddeploy.bsp.root.dir=$(BSP_DIR) $(JAVA_VERBOSE)
	$(call del_file,.*.java.app)
	$(TOUCH) $@

nxpvee-validation.prj: clean .java.mock .java.configuration .java.test.s2s $(addprefix .java.test.,$(VALIDATIONS))
	echo "done"

%.prj: .java.mock .java.configuration .%.java.app
	$(EXPORT) BOARD=$(BOARD) $(LINK) make -C $(BSP_DIR)/projects/$(basename $@)/sdk_makefile

$(addsuffix -javazip,$(PROJS)):
	make clean
	cd $(BSP_DIR)/projects/microej; $(UNZIP) $(JAVAZIP)
	make -C $(BSP_DIR)/projects/$(@:%-javazip=%)/sdk_makefile

$(addsuffix -flash,$(PROJS)):
	make $(@:%-flash=%).prj
	$(EXPORT) BOARD=$(BOARD) $(LINK) make -C $(BSP_DIR)/projects/$(@:%-flash=%)/sdk_makefile flash

$(addsuffix -flash_cmsisdap,$(PROJS)):
	make $(@:%-flash_cmsisdap=%).prj
	$(EXPORT) BOARD=$(BOARD) $(LINK) make -C $(BSP_DIR)/projects/$(@:%-flash_cmsisdap=%)/sdk_makefile flash_cmsisdap

$(addsuffix -gdb,$(PROJS)):
	make $(@:%-gdb=%).prj
	$(EXPORT) BOARD=$(BOARD) $(LINK) make -C $(BSP_DIR)/projects/$(@:%-gdb=%)/sdk_makefile gdb

$(addsuffix -gdb_cmsisdap,$(PROJS)):
	make $(@:%-gdb_cmsisdap=%).prj
	$(EXPORT) BOARD=$(BOARD) $(LINK) make -C $(BSP_DIR)/projects/$(@:%-gdb_cmsisdap=%)/sdk_makefile gdb_cmsisdap

$(addsuffix -java_run,$(PROJS)): .java.mock .java.configuration
	cd $(APP_DIR)  $(LINK) $(MICROEJ_BUILDKIT_PATH_VAR)/bin/mmm run  \
		-Declipse.home="$(ECLIPSE_HOME)" \
		$(MODULE_REPOSITORY_SETTINGS) \
		-Dcom.microej.platformbuilder.architecture.usage=$(USAGE) \
		-Dplatform-loader.target.platform.dir=$(PLAT_DIR)/ \
		-Dapplication.main.class=$(if $(MAIN),$(MAIN),$(MAIN.$(@:%-java_run=%).java.app)) \
		-Dplatform-launcher.platform.dir=$(PLAT_DIR)/source $(JAVA_VERBOSE)

$(addsuffix -java_rebuild,$(PROJS)): .java.mock .java.configuration
	$(call del_file,.*.java.app)
	make .$(@:%-java_rebuild=%).java.app

$(addsuffix -clean,$(PROJS)):
	make -C $(BSP_DIR)/projects/$(@:%-clean=%)/sdk_makefile clean
	make -C $(BSP_DIR)/projects/$(@:%-clean=%)/sdk_makefile distclean
	$(call del_file,.*.java.app)
	$(call del_file,.java.*)
	$(call del_file,javasec.prop*)
	$(call del_dir,$(PLAT_DIR)/build/)
	$(call del_dir,$(PLAT_DIR)/source/)
	$(call del_dir,$(CONF_DIR)/dropins/include)
	$(call del_dir,$(CONF_DIR)/dropins/javaLibs)
	$(call del_dir,$(CONF_DIR)/dropins/mocks)
	$(call del_dir,$(CONF_DIR)/dropins/scripts)
	$(call del_file,$(CONF_DIR)/dropins/workbenchExtension_launchScriptFramework.jar)
	$(call del_file,$(CONF_DIR)/dropins/imagegenerator-vectorimage.jar)
	$(call del_file,$(CONF_DIR)/dropins/license-checker.jar)
	$(call del_dir,$(CONF_DIR)/target~/)
	$(call del_dir,$(FP_DIR)/bin/)
	$(call del_dir,$(FP_DIR)/lib/)
	$(call del_dir,$(FP_DIR)/target~/)
	$(call del_dir,$(APP_DIR)/bin/)
	$(call del_dir,$(APP_DIR)/filesystem/)
	$(call del_dir,$(APP_DIR)/src-adpgenerated/)
	$(call del_dir,$(APP_DIR)/target~/)
	$(call del_dir,$(BSP_DIR)/projects/microej/platform/)
	$(call del_dir,$(MOCK_DIR)/bin/)
	$(call del_dir,$(MOCK_DIR)/target~/)
	$(call del_dir,$(VAL_DIR)/core/java-testsuite-runner-core/target~/)

clean: $(addsuffix -clean,$(PROJS))
	echo "clean"
