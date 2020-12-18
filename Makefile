include $(TOPDIR)/rules.mk

PKG_NAME:=gl-zbtool
PKG_VERSION:=2.2.1
# PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/gl-zbtool
  SECTION:=base
  CATEGORY:=gl-inet
  TITLE:=A framework for GL-iNet zigbee (emberZnet SDK v2.7).
  DEPENDS:=+libuci +libubox +libubus +libblobmsg-json +libpthread +libjson-c 
endef

define Package/gl-zbtool/description
A framework for silicon labs zigbee.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

MAKE_FLAGS += \
		CFLAGS+="-Wall -DSYSLOG"

# 其他应用程序编译依赖则需要InstallDev
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libglzbapi.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libglzbapi.a $(1)/usr/lib/
endef

define Package/gl-zbtool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zbdaemon $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zbtool $(1)/usr/bin/zbtool
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/demoProject $(1)/usr/bin/demoProject
	$(INSTALL_DIR) $(1)/usr/lib/gl
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libglzbapi.so $(1)/usr/lib/gl/libglzbapi.so
	$(LN) /usr/lib/gl/libglzbapi.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/gl-zbtool.init $(1)/etc/init.d/zbdaemon
endef

$(eval $(call BuildPackage,gl-zbtool))
