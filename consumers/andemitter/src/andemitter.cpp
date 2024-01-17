#include <andemitter.hpp>

    void andemitter::notifyachiev (
      name org, 
      name badge,
      name account, 
      name from,
      uint64_t count,
      string memo, 
      vector<name> notify_accounts ) {

      activelookup_table _activelookup( _self, org.value );
      auto activelookup_itr = _activelookup.find(badge.value);
      if(activelookup_itr != _activelookup.end()) {
        for(auto i = 0; i < activelookup_itr->active_emissions.size(); i++) { 
          emitters(org, activelookup_itr->active_emissions[i], account, badge, count); 
        }
      }

    }

    ACTION andemitter::addclaimer (name org, name account, name assetname, uint64_t account_cap, string memo) {
      require_auth(get_self());
      require_recipient(name(NOTIFICATION_CONTRACT_NAME));
    }

    ACTION andemitter::newemission (name org,
      name emission_name,
      std::map<name, uint64_t> emitter_criteria,
      std::map<asset_contract_name, uint64_t> emit_assets,
      bool cyclic) {

      check_internal_auth(name("newemission"));
      
      check(emitter_criteria.size() > 0, "emitter_criteria should have atleast 1 entry ");
      check(emit_assets.size() > 0, "emit_assets should have atleast 1 entry for emission_name " + emission_name.to_string());

      for (const auto& [key, value] : emitter_criteria) { 
        valid_badge(org, key);

        check( !emit_assets.contains(asset_contract_name {
          .issuing_contract=name(SIMPLEBADGE_CONTRACT_NAME),
          .asset_name=key}), "cant handle if emitter_criteria and emit_assets has same asset");

        check( !emit_assets.contains(asset_contract_name {
          .issuing_contract=name(CLAIMASSET_CONTRACT_NAME),
          .asset_name=key}), "cant handle if emitter_criteria and emit_assets has same asset");
      }

      for (const auto& [key, value] : emit_assets) { 
        valid_badge(org, key.asset_name);
      } 

      emissions_table _emissions( _self, org.value );
      auto itr = _emissions.find(emission_name.value);
      check(itr == _emissions.end(), "Emission with this name already exist.");
      _emissions.emplace(get_self(), [&](auto& row){
        row.emission_name = emission_name;
        row.emitter_criteria = emitter_criteria;
        row.emit_assets = emit_assets;
        row.cyclic = cyclic;
        row.status = name("init");
      }); 

    }

    ACTION andemitter::activate (name org, name emission_name) {
      check_internal_auth(name("activate"));
      emissions_table _emissions( _self, org.value );
      auto emissions_itr = _emissions.require_find(emission_name.value, "emission not defined");
      check(emissions_itr->status == name("init"), "emission not in init state");

      _emissions.modify(emissions_itr, get_self(), [&](auto& row){
        row.status = name("activated");
      });


      for (const auto& [key, value] : emissions_itr->emitter_criteria) { 
        activelookup_table _activelookup( _self, org.value );
        name badge = key;
        auto itr = _activelookup.find(badge.value);
        if(itr == _activelookup.end()) {
          vector<name> active_emissions ;
          active_emissions.push_back(emission_name);
          _activelookup.emplace(get_self(), [&](auto& row){
            row.badge = badge;
            row.active_emissions = active_emissions;
          });
        } else {
          vector<name> active_emissions = itr->active_emissions;
          active_emissions.push_back(emission_name);
          _activelookup.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = active_emissions;
          });
        }
      }
    }

    ACTION andemitter::deactivate (name org, name emission_name) {
      check_internal_auth(name("deactivate"));
      emissions_table _emissions( _self, org.value );
      auto emissions_itr = _emissions.require_find(emission_name.value, "emission not defined");
      check(emissions_itr->status == name("init") || emissions_itr->status == name("activated"), "emission not in init/activated state");

      _emissions.modify(emissions_itr, get_self(), [&](auto& row){
        row.status = name("deactivated");
      });

      for (const auto& [key, value] : emissions_itr->emitter_criteria) { 
        activelookup_table _activelookup( _self, org.value );
        auto itr = _activelookup.require_find(key.value, "somethings wrong, should have found an entry");
        vector<name> active_emissions = itr->active_emissions;
        vector<name> new_active_emissions ;
        for(auto i = 0 ; i < active_emissions.size(); i++) {
          if(active_emissions[i] != emission_name) {
            new_active_emissions.push_back(active_emissions[i]);
          }
        }
        if(active_emissions.size() > 0) {
          _activelookup.modify(itr, get_self(), [&](auto& row){
            row.active_emissions = new_active_emissions;
          });
        } else {
          itr = _activelookup.erase(itr);
        }
      }
    }
