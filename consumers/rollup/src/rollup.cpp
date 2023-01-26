#include <rollup.hpp>

    void rollup::notifyachiev (
      name org, 
      name badge_contract, 
      name badge_name,
      name account, 
      name from,
      uint8_t count,
      string memo,
      uint64_t badge_id,  
      vector<name> notify_accounts) {


      activators_table _activators( _self, org.value );
      auto activators_itr = _activators.find(badge_id);
      for(auto i = 0; i < activators_itr->active_emissions.size(); i++) { 
        activators(org, activators_itr->active_emissions[i], account, badge_contract, badge_name, count); 
      }
      emitters_table _emitters( _self, org.value );
      auto emitters_itr = _emitters.find(badge_id);
      for(auto i = 0; i < emitters_itr->active_emissions.size(); i++) { 
        emitters(org, emitters_itr->active_emissions[i], account, badge_contract, badge_name, count); 
      }
    }

    ACTION rollup::issuesimple (name org, name to, name badge, string memo) {
      require_auth(get_self());
      require_recipient(name(NOTIFICATION_CONTRACT));
    }

    ACTION rollup::addclaimer (name org, name account, name assetname, uint64_t account_cap) {
      require_auth(get_self());
      require_recipient(name(NOTIFICATION_CONTRACT));
    }

    ACTION rollup::createrollup (name org,
      name emission_name,
      std::map<asset_contract_name, uint16_t> activator_criteria,
      std::map<asset_contract_name, uint16_t> emitter_criteria,
      std::map<asset_contract_name, uint16_t> emit_assets,
      bool cyclic_activator,
      bool cyclic_emitter) {

      require_auth(get_self());
      emissions_table _emissions( _self, org.value );
      auto itr = _emissions.find(emission_name.value);
      check(itr == _emissions.end(), "Rollup with this name already exist.");
      _emissions.emplace(get_self(), [&](auto& row){
        row.emission_name = emission_name;
        row.activator_criteria = activator_criteria;
        row.emitter_criteria = emitter_criteria;
        row.emit_assets = emit_assets;
        row.cyclic_activator = cyclic_activator;
        row.cyclic_emitter = cyclic_emitter;
        row.status = name("init");
      }); 

      uint64_t badge_id;
      for (const auto& [key, value] : activator_criteria) { 
        badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
      }

      for (const auto& [key, value] : emitter_criteria) { 
        badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
      }

      for (const auto& [key, value] : emit_assets) { 
        badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
      }  
    }

    ACTION rollup::activate (name org, name emission_name) {
      emissions_table _emissions( _self, org.value );
      auto emissions_itr = _emissions.require_find(emission_name.value, "emission not defined");
      check(emissions_itr->status == name("init"), "rollup not in init state");

      _emissions.modify(emissions_itr, get_self(), [&](auto& row){
        row.status = name("activated");
      });

      for (const auto& [key, value] : emissions_itr->activator_criteria) { 
        uint64_t badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
        activators_table _activators( _self, org.value );
        auto itr = _activators.find(badge_id);
        if(itr == _activators.end()) {
          vector<name> active_emissions ;
          active_emissions.push_back(emission_name);
          _activators.emplace(get_self(), [&](auto& row){
            row.badge_id = badge_id;
            row.active_emissions = active_emissions;
          });
        } else {
          vector<name> active_emissions = itr->active_emissions;
          active_emissions.push_back(emission_name);
          _activators.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = active_emissions;
          });
        }
      }

      for (const auto& [key, value] : emissions_itr->emitter_criteria) { 
        uint64_t badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
        emitters_table _emitters( _self, org.value );
        auto itr = _emitters.find(badge_id);
        if(itr == _emitters.end()) {
          vector<name> active_emissions ;
          active_emissions.push_back(emission_name);
          _emitters.emplace(get_self(), [&](auto& row){
            row.badge_id = badge_id;
            row.active_emissions = active_emissions;
          });
        } else {
          vector<name> active_emissions = itr->active_emissions;
          active_emissions.push_back(emission_name);
          _emitters.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = active_emissions;
          });
        }
      }
    }

    ACTION rollup::deactivate (name org, name emission_name) {
      emissions_table _emissions( _self, org.value );
      auto emissions_itr = _emissions.require_find(emission_name.value, "emission not defined");
      check(emissions_itr->status == name("init") || emissions_itr->status == name("activate"), "emission not in init/activated state");

      _emissions.modify(emissions_itr, get_self(), [&](auto& row){
        row.status = name("deactivated");
      });

      for (const auto& [key, value] : emissions_itr->activator_criteria) { 
        uint64_t badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
        activators_table _activators( _self, org.value );
        auto itr = _activators.require_find(badge_id, "somethings wrong, should have found an entry");
        vector<name> active_emissions = itr->active_emissions;
        vector<name> new_active_emissions ;
        for(auto i = 0 ; i < active_emissions.size(); i++) {
          if(active_emissions[i] != emission_name) {
            new_active_emissions.push_back(active_emissions[i]);
          }
        }
        if(active_emissions.size() > 0) {
          _activators.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = new_active_emissions;
          });
        } else {
          itr = _activators.erase(itr);
        }
      }

      for (const auto& [key, value] : emissions_itr->emitter_criteria) { 
        uint64_t badge_id = get_badge_id(org, key.issuing_contract, key.asset_name);
        emitters_table _emitters( _self, org.value );
        auto itr = _emitters.require_find(badge_id, "somethings wrong, should have found an entry");
        vector<name> active_emissions = itr->active_emissions;
        vector<name> new_active_emissions ;
        for(auto i = 0 ; i < active_emissions.size(); i++) {
          if(active_emissions[i] != emission_name) {
            new_active_emissions.push_back(active_emissions[i]);
          }
        }
        if(active_emissions.size() > 0) {
          _emitters.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = new_active_emissions;
          });
        } else {
          itr = _emitters.erase(itr);
        }
      }
    }