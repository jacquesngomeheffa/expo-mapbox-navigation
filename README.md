# @jacques_gordon/expo-mapbox-navigation

Plugin Expo pour la navigation Mapbox **turn-by-turn**, compatible **Expo SDK 53** avec **Expo Dev Client**.

Supporte **iOS** et **Android**, avec waypoints multiples, instructions vocales, reroutage automatique, restrictions de gabarit de véhicule et personnalisation de la carte.

> ⚠️ Nécessite **Expo Dev Client** — incompatible avec Expo Go.

---

## Fonctionnalités

- ✅ Navigation turn-by-turn complète (iOS & Android)
- ✅ Waypoints multiples
- ✅ Instructions vocales
- ✅ Reroutage automatique
- ✅ Détection hors-route
- ✅ Restrictions de gabarit de véhicule (hauteur / largeur)
- ✅ Map Matching API (trajet explicite selon les coordonnées)
- ✅ Routes alternatives
- ✅ Style de carte personnalisable
- ✅ Couche raster personnalisée
- ✅ Multilangue
- ✅ Compatible avec `@rnmapbox/maps`

---

## Installation

### 1. Installer `@rnmapbox/maps` (dépendance requise)

Suivre les [instructions officielles de @rnmapbox/maps](https://rnmapbox.github.io/docs/install).  
La version recommandée est `11.11.0`. **Important : définir explicitement `RNMapboxMapsVersion`.**

### 2. Installer le package

```bash
npm install @jacques_gordon/expo-mapbox-navigation
```

### 3. Configurer le plugin dans `app.json`

```json
{
  "expo": {
    "plugins": [
      [
        "expo-build-properties",
        {
          "ios": {
            "useFrameworks": "static"
          }
        }
      ],
      [
        "@jacques_gordon/expo-mapbox-navigation",
        {
          "accessToken": "pk.eyJ1IjoiVk9UUkVfVE9LRU4iLCJhIjoiY...",
          "mapboxMapsVersion": "11.11.0",
          "androidColorOverrides": {
            "mapbox_main_maneuver_background_color": "#1E88E5"
          }
        }
      ]
    ]
  }
}
```

### 4. Configurer les tokens secrets

#### iOS — `~/.netrc`

```
machine api.mapbox.com
  login mapbox
  password sk.eyJ1IjoiVk9UUkVfVE9LRU5fU0VDUkVUIiwiYSI6Ii4uLiJ9...
```

#### Android — `~/.gradle/gradle.properties`

```properties
MAPBOX_DOWNLOADS_TOKEN=sk.eyJ1IjoiVk9UUkVfVE9LRU5fU0VDUkVUIiwiYSI6Ii4uLiJ9...
```

### 5. Prebuild + Dev Client

```bash
npx expo prebuild --clean
npx expo run:ios     # ou run:android
```

---

## Utilisation

### Exemple de base

```tsx
import { MapboxNavigationView } from '@jacques_gordon/expo-mapbox-navigation';

export default function NavigationScreen() {
  return (
    <MapboxNavigationView
      style={{ flex: 1 }}
      coordinates={[
        { latitude: 48.8566, longitude: 2.3522 },  // Paris — départ
        { latitude: 43.2965, longitude: 5.3698 },  // Marseille — destination
      ]}
      locale="fr"
      onFinalDestinationArrival={() => console.log('Arrivé !')}
      onCancelNavigation={() => navigation.goBack()}
    />
  );
}
```

### Exemple complet (compatible avec la page de navigation)

```tsx
import { MapboxNavigationView } from '@jacques_gordon/expo-mapbox-navigation';

export default function NavigationScreen({ navigation, reservation, currentPosition }) {
  const coordinates = [
    currentPosition,                              // départ (position actuelle)
    { latitude: 45.7640, longitude: 4.8357 },    // Lyon — waypoint intermédiaire
    reservation.address_to_lat_long,              // destination finale
  ];

  return (
    <MapboxNavigationView
      style={{ flex: 1 }}
      coordinates={coordinates}
      locale="fr"
      vehicleMaxHeight={5.0}
      vehicleMaxWidth={2.5}
      showsEndOfRouteFeedback
      onRouteProgressChanged={async (event) => {
        const { distanceRemaining, durationRemaining, distanceTraveled, fractionTraveled } =
          event.nativeEvent;

        // Notifier le client que le chauffeur approche
        if (distanceRemaining <= 60) {
          AXIOS_POST('reservations/livetracking/drivernear', { reservation });
        }

        const eta = new Date(Date.now() + durationRemaining * 1000);
        dispatch(setNavigationData({ eta, distanceRemaining, durationRemaining }));
      }}
      onWaypointArrival={(event) => {
        console.log('Waypoint atteint !', event.nativeEvent);
      }}
      onFinalDestinationArrival={async () => {
        await playSound(SOUNDS.ROAD_RECALCULATE);
        showToast({ toast_type: 'validation', title: 'Arrivée', body: 'Destination atteinte !' });
      }}
      onCancelNavigation={() => navigation.goBack()}
      onRouteChanged={() => console.log('Reroutage en cours...')}
      onUserOffRoute={() => console.log('Hors route !')}
      onRoutesLoaded={() => console.log('Routes chargées')}
    />
  );
}
```

---

## Props

### Route

| Prop | Type | Requis | Description |
|------|------|--------|-------------|
| `coordinates` | `Coordinate[]` | **Oui** | Tableau de `{latitude, longitude}`. Minimum 2 points. |
| `waypointIndices` | `number[]` | Non | Indices des points considérés comme waypoints. Par défaut: tous. |

### Options de routing

| Prop | Type | Défaut | Description |
|------|------|--------|-------------|
| `locale` | `string` | locale appareil | Langue des instructions. Ex: `"fr"`, `"en"`, `"de"` |
| `routeProfile` | `string` | `"mapbox/driving-traffic"` | Profil de routing. Android: sans `"mapbox/"`. |
| `routeExcludeList` | `string[]` | `[]` | Routes à exclure: `"toll"`, `"ferry"`, `"motorway"` |
| `useRouteMatchingApi` | `boolean` | `false` | Utilise l'API Map Matching au lieu de l'API Directions |
| `disableAlternativeRoutes` | `boolean` | `false` | Désactive les routes alternatives |
| `mute` | `boolean` | `false` | Coupe le son de navigation au démarrage |
| `vehicleMaxHeight` | `number` | — | Hauteur max du véhicule en mètres |
| `vehicleMaxWidth` | `number` | — | Largeur max du véhicule en mètres |

### Personnalisation visuelle

| Prop | Type | Défaut | Description |
|------|------|--------|-------------|
| `mapStyle` | `string` | style par défaut | URL du style Mapbox |
| `customRasterSourceUrl` | `string` | — | URL template raster personnalisée (ex: OpenStreetMap) |
| `placeCustomRasterLayerAbove` | `string` | — | ID du layer au-dessus duquel placer la couche raster |
| `showsEndOfRouteFeedback` | `boolean` | `false` | Affiche l'UI de feedback à l'arrivée |

### Callbacks

| Prop | Paramètres | Description |
|------|-----------|-------------|
| `onRouteProgressChanged` | `{ nativeEvent: RouteProgress }` | Progression sur la route (appelé fréquemment) |
| `onWaypointArrival` | `{ nativeEvent: RouteProgress }` (Android only) | Arrivée à un waypoint intermédiaire |
| `onFinalDestinationArrival` | — | Arrivée à la destination finale |
| `onCancelNavigation` | — | Annulation de la navigation par l'utilisateur |
| `onRouteChanged` | — | Route modifiée ou reroutage |
| `onUserOffRoute` | — | L'utilisateur est sorti de la route |
| `onRoutesLoaded` | — | Routes chargées et prêtes |

---

## Types TypeScript

```ts
interface Coordinate {
  latitude: number;
  longitude: number;
}

interface RouteProgress {
  distanceRemaining: number;   // mètres restants
  distanceTraveled: number;    // mètres parcourus
  durationRemaining: number;   // secondes restantes
  fractionTraveled: number;    // 0.0 → 1.0
}
```

---

## Plugin `app.json` — Options complètes

```json
[
  "@jacques_gordon/expo-mapbox-navigation",
  {
    "accessToken": "pk.eyJ1...",
    "mapboxMapsVersion": "11.11.0",
    "androidColorOverrides": {
      "mapbox_main_maneuver_background_color": "#1E88E5",
      "mapbox_sub_maneuver_background_color": "#1565C0",
      "mapbox_banner_background_color": "#FFFFFF"
    }
  }
]
```

---

## Publier sur npm

```bash
# 1. Compiler le module TypeScript
npm run build

# 2. Se connecter à npm
npm login

# 3. Publier
npm publish --access public
```

> Remplacer `@jacques_gordon` par votre scope npm (ex: `@monentreprise`).

---

## Notes importantes

- Nécessite **Expo Dev Client** (Expo Go non supporté)
- iOS minimum : **14.0**
- Android minimum SDK : **24**
- Le SDK Mapbox Navigation est propriétaire — facturation via votre compte Mapbox
- Pour iOS, les `.xcframework` du SDK Mapbox Navigation doivent être compilés et inclus dans `ios/Frameworks/`
  (voir [instructions dans le README du repo source](https://github.com/uju777/expo-mapbox-navigation))

---

## Obtenir vos tokens Mapbox

1. Créer un compte sur [mapbox.com](https://www.mapbox.com/)
2. Aller dans [Account > Tokens](https://account.mapbox.com/access-tokens/)
3. **Token public** (`pk.eyJ1...`) — token par défaut ou nouveau token
4. **Token secret** (`sk.eyJ1...`) — nouveau token avec le scope `Downloads:Read`

---

## Licence

MIT
