/*
 * 🧱 File: SystemUuidGen.h
 * ------------------------
 * 📌 Purpose   : Production UUID generator using Qt QUuid.
 * 🧱 Layer     : Infrastructure (Utils)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Implements IUuidGen using QUuid from Qt.
 */

#pragma once

#include "domain/core/IUuidGen.h"
#include <QUuid>

class SystemUuidGen : public IUuidGen
{
public:
  // 🔑 Generate a new UUID (QString)
  QString generate() override { return QUuid::createUuid().toString(QUuid::WithoutBraces); }
};
