<template>
  <v-app>
    <v-app-bar app color="primary" dark>
      <div class="d-flex align-center">
        <v-icon width="40">mdi-led-on</v-icon>
        WS281x Controller
      </div>

      <v-spacer></v-spacer>

      <v-btn text @click="showConfiguration" v-if="pageVisible == 'dashboard'">
        <span class="mr-2">Configuration</span>
        <v-icon>mdi-cog</v-icon>
      </v-btn>
      <v-btn text @click="showDashboard" v-if="pageVisible == 'configuration'">
        <span class="mr-2">Dashboard</span>
        <v-icon>mdi-view-dashboard</v-icon>
      </v-btn>
    </v-app-bar>

    <v-main>
      <Dashboard v-if="pageVisible == 'dashboard'" />
      <Configuration v-if="pageVisible == 'configuration'" />
    </v-main>
  </v-app>
</template>

<script>
import Dashboard from "./components/Dashboard";
import Configuration from "./components/Configuration";

export default {
  name: "App",

  components: {
    Dashboard,
    Configuration
  },

  data: () => ({
    pageVisible: "dashboard"
  }),
  methods: {
    showDashboard() {
      this.pageVisible = "dashboard";
    },
    showConfiguration() {
      this.pageVisible = "configuration";
    }
  }
};
</script>
