import type { StyleProp, ViewStyle } from 'react-native';

// ─── Coordonnée de base ──────────────────────────────────────────────────────

export interface Coordinate {
  latitude: number;
  longitude: number;
}

// ─── Progression sur la route ─────────────────────────────────────────────────

export interface RouteProgress {
  /** Distance restante en mètres */
  distanceRemaining: number;
  /** Distance parcourue en mètres */
  distanceTraveled: number;
  /** Durée restante en secondes */
  durationRemaining: number;
  /** Fraction parcourue (0.0 → 1.0) */
  fractionTraveled: number;
}

// ─── Événement natif (nativeEvent wrapper) ───────────────────────────────────

export interface NativeRouteProgressEvent {
  nativeEvent: RouteProgress;
}

export interface NativeWaypointEvent {
  nativeEvent: RouteProgress;
}

// ─── Props du composant ───────────────────────────────────────────────────────

export interface MapboxNavigationViewProps {
  /** Style du composant (ex: { flex: 1 }) */
  style?: StyleProp<ViewStyle>;

  // ─── Route ─────────────────────────────────────────────────────────────

  /**
   * Tableau de coordonnées { latitude, longitude }.
   * Minimum 2 points (départ + destination).
   * Requis.
   */
  coordinates: Coordinate[];

  /**
   * Indices dans `coordinates` considérés comme des waypoints/destinations intermédiaires.
   * Par défaut, tous les points sont des waypoints.
   * Au moins le premier et le dernier index doivent être inclus.
   */
  waypointIndices?: number[];

  // ─── Options de routing ────────────────────────────────────────────────

  /**
   * Utilise l'API Map Matching de Mapbox au lieu de l'API Directions standard.
   * Utile pour forcer un trajet suivant exactement les coordonnées fournies.
   * Défaut: false
   */
  useRouteMatchingApi?: boolean;

  /**
   * Locale/langue pour les labels de la carte, les directions et la voix.
   * Exemples: "fr", "en", "de", "nl".
   * Par défaut: locale de l'appareil.
   */
  locale?: string;

  /**
   * Profil de routing Mapbox.
   * iOS: "mapbox/driving-traffic" | "mapbox/driving" | "mapbox/walking" | "mapbox/cycling"
   * Android: "driving-traffic" | "driving" | "walking" | "cycling" (sans "mapbox/" prefix)
   * Défaut: "mapbox/driving-traffic"
   */
  routeProfile?: string;

  /**
   * Types de routes à exclure du calcul d'itinéraire.
   * Exemples: ["toll", "ferry", "motorway"]
   */
  routeExcludeList?: string[];

  /**
   * Style de la carte Mapbox.
   * Exemples: "mapbox://styles/mapbox/navigation-day-v1"
   */
  mapStyle?: string;

  /**
   * Coupe le son de navigation au démarrage.
   * Défaut: false
   */
  mute?: boolean;

  /**
   * Hauteur maximale du véhicule en mètres.
   * Permet d'éviter les routes avec restriction de hauteur.
   */
  vehicleMaxHeight?: number;

  /**
   * Largeur maximale du véhicule en mètres.
   * Permet d'éviter les routes avec restriction de largeur.
   */
  vehicleMaxWidth?: number;

  /**
   * URL d'une source raster personnalisée pour la carte.
   * Doit être une URL template avec {x}, {y}, {z}.
   * Exemple: "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
   */
  customRasterSourceUrl?: string;

  /**
   * ID du layer au-dessus duquel placer le layer raster personnalisé.
   */
  placeCustomRasterLayerAbove?: string;

  /**
   * Désactive le calcul et l'affichage des routes alternatives.
   * Défaut: false
   */
  disableAlternativeRoutes?: boolean;

  /**
   * Affiche l'UI de feedback en fin de route.
   * Défaut: false
   */
  showsEndOfRouteFeedback?: boolean;

  // ─── Callbacks ─────────────────────────────────────────────────────────

  /**
   * Appelé régulièrement avec la progression sur la route.
   * L'objet est dans event.nativeEvent.
   */
  onRouteProgressChanged?: (event: NativeRouteProgressEvent) => void;

  /**
   * Appelé quand l'utilisateur arrive à un waypoint intermédiaire.
   * Sur Android uniquement: fournit les données de progression dans event.nativeEvent.
   */
  onWaypointArrival?: (event: NativeWaypointEvent) => void;

  /**
   * Appelé quand l'utilisateur arrive à la destination finale.
   */
  onFinalDestinationArrival?: () => void;

  /**
   * Appelé quand l'utilisateur appuie sur le bouton d'annulation.
   * La navigation ne se ferme pas automatiquement — gérer manuellement (ex: navigation.goBack()).
   */
  onCancelNavigation?: (event?: any) => void;

  /**
   * Appelé quand la route change ou qu'un reroutage est effectué.
   */
  onRouteChanged?: () => void;

  /**
   * Appelé quand l'utilisateur sort de la route.
   */
  onUserOffRoute?: () => void;

  /**
   * Appelé quand les routes sont chargées et prêtes.
   */
  onRoutesLoaded?: () => void;
}
