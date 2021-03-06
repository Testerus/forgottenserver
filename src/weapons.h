/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_WEAPONS_H_69D1993478AA42948E24C0B90B8F5BF5
#define FS_WEAPONS_H_69D1993478AA42948E24C0B90B8F5BF5

#include "luascript.h"
#include "player.h"
#include "baseevents.h"
#include "combat.h"
#include "const.h"

class Weapon;
class WeaponMelee;
class WeaponDistance;
class WeaponWand;

class Weapons final : public BaseEvents
{
	public:
		Weapons();
		~Weapons();

		void loadDefaults();
		const Weapon* getWeapon(const Item* item) const;

		static int32_t getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue);
		static int32_t getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor);

	protected:
		void clear() final;
		LuaScriptInterface& getScriptInterface() final;
		std::string getScriptBaseName() const final;
		Event* getEvent(const std::string& nodeName) final;
		bool registerEvent(Event* event, const pugi::xml_node& node) final;

		std::map<uint32_t, Weapon*> weapons;

		LuaScriptInterface m_scriptInterface;
};

class Weapon : public Event
{
	public:
		Weapon(LuaScriptInterface* _interface);
		~Weapon();

		bool configureEvent(const pugi::xml_node& node) override;
		bool loadFunction(const std::string& functionName) final;
		virtual bool configureWeapon(const ItemType& it);
		virtual bool interruptSwing() const {
			return false;
		}

		virtual int32_t playerWeaponCheck(Player* player, Creature* target) const;
		static bool useFist(Player* player, Creature* target);
		virtual bool useWeapon(Player* player, Item* item, Creature* target) const;

		virtual int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const = 0;
		virtual int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const = 0;
		virtual CombatType_t getElementType() const = 0;

		uint16_t getID() const {
			return id;
		}

		uint32_t getReqLevel() const {
			return level;
		}
		uint32_t getReqMagLv() const {
			return magLevel;
		}
		bool isPremium() const {
			return premium;
		}
		bool isWieldedUnproperly() const {
			return wieldUnproperly;
		}

	protected:
		std::string getScriptEventName() const final;

		bool executeUseWeapon(Player* player, const LuaVariant& var) const;
		bool internalUseWeapon(Player* player, Item* item, Creature* target, int32_t damageModifier) const;
		bool internalUseWeapon(Player* player, Item* item, Tile* tile) const;

		void onUsedWeapon(Player* player, Item* item) const;
		virtual void onUsedAmmo(Item* item, Tile* destTile) const;
		virtual bool getSkillType(const Player*, const Item*, skills_t&, uint32_t&) const {
			return false;
		}

		uint32_t getManaCost(const Player* player) const;

		CombatParams params;

		uint32_t level;
		uint32_t magLevel;
		uint32_t mana;
		uint32_t manaPercent;
		uint32_t soul;
		uint16_t id;
		AmmoAction_t ammoAction;
		uint8_t range;
		bool enabled;
		bool premium;
		bool wieldUnproperly;

	private:
		std::map<uint16_t, bool> vocWeaponMap;
		friend class Combat;
};

class WeaponMelee final : public Weapon
{
	public:
		WeaponMelee(LuaScriptInterface* _interface);
		~WeaponMelee() {}

		bool configureWeapon(const ItemType& it) final;

		bool useWeapon(Player* player, Item* item, Creature* target) const final;

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const final;
		int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const final;
		CombatType_t getElementType() const final { return elementType; }

	protected:
		bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const final;

		CombatType_t elementType;
		int16_t elementDamage;
};

class WeaponDistance final : public Weapon
{
	public:
		WeaponDistance(LuaScriptInterface* _interface);
		~WeaponDistance() {}

		bool configureEvent(const pugi::xml_node& node) final;
		bool configureWeapon(const ItemType& it) final;
		bool interruptSwing() const final {
			return true;
		}

		int32_t playerWeaponCheck(Player* player, Creature* target) const final;
		bool useWeapon(Player* player, Item* item, Creature* target) const final;

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const final;
		int32_t getElementDamage(const Player* player, const Creature* target, const Item* item) const final;
		CombatType_t getElementType() const final { return elementType; }

	protected:
		void onUsedAmmo(Item* item, Tile* destTile) const final;
		bool getSkillType(const Player* player, const Item* item, skills_t& skill, uint32_t& skillpoint) const final;

		int32_t hitChance;
		int32_t maxHitChance;
		int32_t breakChance;
		int32_t ammuAttackValue;

		CombatType_t elementType;
		int16_t elementDamage;
};

class WeaponWand final : public Weapon
{
	public:
		WeaponWand(LuaScriptInterface* _interface);
		~WeaponWand() {}

		bool configureEvent(const pugi::xml_node& node) final;
		bool configureWeapon(const ItemType& it) final;

		int32_t getWeaponDamage(const Player* player, const Creature* target, const Item* item, bool maxDamage = false) const final;
		int32_t getElementDamage(const Player*, const Creature*, const Item*) const final { return 0; }
		CombatType_t getElementType() const final { return COMBAT_NONE; }

	protected:
		bool getSkillType(const Player*, const Item*, skills_t&, uint32_t&) const final {
			return false;
		}

		int32_t minChange;
		int32_t maxChange;
};

#endif
