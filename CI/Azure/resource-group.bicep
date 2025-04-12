//  Parameters
@description('The target environment. Provide one of the allowed values.')
@allowed(['Development', 'Test', 'Production'])
param environment string

param location string = resourceGroup().location

var environmentSuffix = toLower(substring(environment, 0, 1))

@description('The App Service Plan SKU name.')
var appServicePlanSkuName = (environment == 'Production') ? 'B1' : 'B1'

var tags = {
  Environment: environment
  Creator: 'Robert Herber'
}

// Resources
@description('The App Service Plan for web app')
resource appServicePlanWA 'Microsoft.Web/serverfarms@2022-09-01' = {
  name: 'rherber-development'
  location: location
  sku: {
    name: appServicePlanSkuName
  }
  tags: tags
  kind: 'app'
  properties: {
    perSiteScaling: false
    elasticScaleEnabled: false
    maximumElasticWorkerCount: 1
    isSpot: false
    reserved: false
    isXenon: false
    hyperV: false
    targetWorkerCount: 0
    targetWorkerSizeId: 0
    zoneRedundant: false
  }
}

@description('The App Service')
resource iotLightSwitchApiAppService 'Microsoft.Web/sites@2022-09-01' = {
  name: 'iotlightswitch-api-uw-wa-${environmentSuffix}'
  location: location
  tags: tags
  kind: 'app'
  identity: {
    type: 'SystemAssigned'
  }
  properties: {
    serverFarmId: appServicePlanWA.id
    httpsOnly: true
    publicNetworkAccess: 'Enabled'
    redundancyMode: 'None'
    clientAffinityEnabled: true
    enabled: true
    clientCertEnabled: false
    siteConfig: {
      netFrameworkVersion: '8.0'
      numberOfWorkers: 1
      appSettings: [
        {
          name: 'APPINSIGHTS_INSTRUMENTATIONKEY'
          value: applicationInsights.properties.InstrumentationKey
        }
        {
          name: 'APPLICATIONINSIGHTS_CONNECTION_STRING'
          value: applicationInsights.properties.ConnectionString
        }
        {
          name: 'ApplicationInsightsAgent_EXTENSION_VERSION'
          value: '~3'
        }
        {
          name: 'ASPNETCORE_ENVIRONMENT'
          value: environment
        }
        {
          name: 'XDT_MicrosoftApplicationInsights_Mode'
          value: 'Recommended'
        }
      ]
    }
  }
}

@description('The Workspace for Application Insights')
resource workspaceForApplicationInsights 'Microsoft.OperationalInsights/workspaces@2022-10-01' = {
  name: 'iotlightswitch-uw-ws-${environmentSuffix}'
  location: location
  tags: tags
}

@description('The Application Insights')
resource applicationInsights 'Microsoft.Insights/components@2020-02-02' = {
  name: 'iotlightswitch-uw-ai-${environmentSuffix}'
  location: location
  tags: tags
  kind: 'web'
  properties: {
    Application_Type: 'web'
    publicNetworkAccessForIngestion: 'Enabled'
    publicNetworkAccessForQuery: 'Enabled'
    IngestionMode: 'LogAnalytics'
    WorkspaceResourceId: workspaceForApplicationInsights.id
    SamplingPercentage: 100
  }
}

output iotLightSwitchApiHostName string = iotLightSwitchApiAppService.properties.defaultHostName
