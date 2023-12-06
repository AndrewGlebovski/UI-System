/**
 * @file Plugin.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_PLUGIN_H
#define __PLUG_PLUGIN_H

#include <cstddef>

namespace plug {

/**
 * @brief Interface ids for predefined interfaces
 */
enum class PluginGuid {
  Plugin = 0, /*!< Default plugin interface */
  Tool = 1,   /*!< Tool interface */
  Filter = 2, /*!< Filter interface */
};

/**
 * @brief Common plugin data
 */
class PluginData {
public:
  /**
   * @brief Retrieve plugin name
   *
   * @return Plugin name as C-string
   */
  virtual const char *getName(void) const = 0;

  /**
   * @brief Retrieve path to plugin icon texture file
   *
   * @return Path to texture file as C-string
   */
  virtual const char *getTexturePath(void) const = 0;

  virtual ~PluginData() {}
};

/**
 * @brief Default interface for all plugins
 */
class Plugin {
public:
  /**
   * @brief Retrive some other interface implemented by plugin
   *
   * @param[in] interface_id	Id of requested interface
   *
   * @return If requested interface is implemented, return object implementing
   *         this interface. Otherwise, return `nullptr`
   */
  virtual Plugin *tryGetInterface(size_t interface_id) = 0;

  /**
   * @brief Increment reference counter on interface instance
   */
  virtual void addReference(void) = 0;

  /**
   * @brief Decrement reference counter on interface instance and destroy object
   *        if necessary
   */
  virtual void release(void) = 0;

  /**
   * @brief Retrieve pointer to plugin interface data
   *
   * @return Pointer to data specific to interface
   */
  virtual const PluginData *getPluginData(void) const = 0;

  virtual ~Plugin() {}
};

} // namespace plug

#endif /* Plugin.h */
