import numpy as np
import matplotlib.pyplot as plt
import datetime
import skyfield.api as sf
import requests
import os
import logging

logger = logging.getLogger()
logger.setLevel(logging.INFO)

# load observer data from environment
logging.info("Loading environment variables.")
spacetrack_id = os.environ["ST_EMAIL"]
spacetrack_pw = os.environ["ST_PASSWORD"]
obs_lat = float(os.environ["OBS_LAT"])
obs_lon = float(os.environ["OBS_LON"])
obs_ele = float(os.environ["OBS_ELE"])

# load range data file
logging.info("Reading measurement data.")
range_data = np.loadtxt("range_data.csv")
range_ts = np.asarray(range_data[:,0], dtype='datetime64[ms]')
range_km_meas = range_data[:,1]
range_km_tle = np.zeros_like(range_km_meas)

# acquire latest TLE before first observation from space-track
logging.info("Retrieving TLE.")
epoch = str(range_ts[0].astype(datetime.datetime).replace(tzinfo=sf.utc))
login_url = "https://www.space-track.org/ajaxauth/login"
login_data = {"identity": spacetrack_id, "password": spacetrack_pw}
query_url = "https://www.space-track.org/basicspacedata/query/class/tle/EPOCH/<" + epoch + "/NORAD_CAT_ID/43700/orderby/EPOCH desc/limit/1/emptyresult/show"

login_response = requests.post(login_url, data=login_data)
response = requests.get(query_url, cookies=login_response.cookies)
data = response.json()
tle_1 = data[0]["TLE_LINE1"]
tle_2 = data[0]["TLE_LINE2"]

# define observer-satellite geometry
logging.info("Calculating satellite-observer geometry.")
obs = sf.Topos(latitude_degrees=obs_lat, longitude_degrees=obs_lon, elevation_m=obs_ele)
sat = sf.EarthSatellite(tle_1, tle_2, name="SAT")
sat_to_obs = sat - obs

# calculate distance for all timesteps
ts = sf.load.timescale()
for idx, time in enumerate(range_ts):
    t = ts.utc(time.astype(datetime.datetime).replace(tzinfo=sf.utc))
    dist = sat_to_obs.at(t).altaz()[2]
    range_km_tle[idx] = dist.km

# calculate residuals
error = range_km_meas - range_km_tle

# plot results
logging.info("Plotting results.")
ax1 = plt.subplot(2, 1, 1)
plt.title("Es'Hail 2 ranging results - mean error: %.02f km" % np.mean(error))
plt.plot(range_ts, range_km_tle, color='black', label="Prediction")
plt.plot(range_ts, range_km_meas, '.', color='black', label="Measurement")
plt.grid()
plt.legend()
plt.ylabel("Slant range [km]")
plt.ylim((np.min(range_km_tle)-5, +np.max(range_km_tle)+10))
plt.xlim((min(range_ts), max(range_ts)))

ax2 = plt.subplot(2, 1, 2, sharex=ax1)
plt.plot(range_ts, error, color='black', label="Residuals")
plt.xlabel("Time")
plt.ylabel("Residuals [km]")
plt.grid()
plt.legend()

plt.show()
