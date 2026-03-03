import { Api } from "./api.js";
import { login } from "./login.js";
import { format_mem, set_text} from "./util.js";

async function fetch_json(url) {
    const response = await fetch(url);
    return await response.json();
}


async function engine_info(api) {
    const data = await api.version();
    set_text("#engine_name", data.Platform.Name);
    set_text("#engine_version", data.Version);
}

async function system_info(api) {
    const data = await api.info();
    set_text("#system_os", data.OperatingSystem);
    set_text("#system_arch", data.Architecture);
    set_text("#system_ncpu", data.NCPU);
    set_text("#system_memory", format_mem(data.MemTotal));
}

async function disk_usage(api) {
    const data = await api.disk_usage();

    set_text("#df_container_count", data.ContainerUsage.TotalCount);
    set_text("#df_container_total", format_mem(data.ContainerUsage.TotalSize));
    set_text("#df_container_reclaimable", format_mem(data.ContainerUsage.Reclaimable));

    set_text("#df_image_count", data.ImageUsage.TotalCount);
    set_text("#df_image_total", format_mem(data.ImageUsage.TotalSize));
    set_text("#df_image_reclaimable", format_mem(data.ImageUsage.Reclaimable));

    set_text("#df_volume_count", data.VolumeUsage.TotalCount);
    set_text("#df_volume_total", format_mem(data.VolumeUsage.TotalSize));
    set_text("#df_volume_reclaimable", format_mem(data.VolumeUsage.Reclaimable));

    set_text("#df_bcache_count", data.BuildCacheUsage.TotalCount);
    set_text("#df_bcache_total", format_mem(data.BuildCacheUsage.TotalSize));
    set_text("#df_bcache_reclaimable", format_mem(data.BuildCacheUsage.Reclaimable));
}


async function containers(api) {
    const data = await api.containers();
    const element = document.querySelector("#containers_list");
    element.innerHTML = "";

    for(const container of data) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const name = document.createElement("td");
        name.textContent = container.Names[0].substring(1);
        tr.appendChild(name);

        const state = document.createElement("td");
        state.textContent = container.State;
        tr.appendChild(state);

        const image = document.createElement("td");
        image.textContent = container.Image;
        tr.appendChild(image);
    }    
}

async function images(api) {
    const data = await api.images();
    const element = document.querySelector("#images_list");

    for(const image of data) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const tags_id = document.createElement("td");
        tr.appendChild(tags_id);

        const tags = document.createElement("div");
        tags.textContent = image.RepoTags.join(" ");
        tags_id.appendChild(tags);

        const id = document.createElement("div");
        id.textContent = image.Id;
        tags_id.appendChild(id);

        const created = document.createElement("td");
        created.textContent = new Date(image.Created * 1000).toISOString();
        tr.appendChild(created);

        const size = document.createElement("td");
        size.textContent = format_mem(image.Size);
        tr.appendChild(size);

    }
}

async function volumes(api) {
    const data = await api.volumes();
    const element = document.querySelector("#volumes_list");
    element.innerHTML = "";

    for(const volume of data.Volumes) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const name = document.createElement("td");
        name.textContent = volume.Name;
        tr.appendChild(name);

        const created = document.createElement("td");
        created.textContent = volume.CreatedAt;
        tr.appendChild(created);
    }
}

function activate_entry(name) {
    const menuentries = document.querySelectorAll(".mainmenu li");
    for(const entry of menuentries) {
        entry.classList.remove("active");
    }
    const entry = document.querySelector(`#menuentry_${name}`);
    entry.classList.add("active");

    const pages = document.querySelectorAll(".page");
    for(const page of pages) {
        page.classList.add("hidden");
    }

    const home = document.querySelector(`#${name}`);
    home.classList.remove("hidden");
}

function init_home(api)
{
    document.querySelector("#menuentry_home").addEventListener("click", () => {
        activate_home(api);
    });

    document.querySelector("#home_containers").addEventListener("click", () => {
        activate_containers(api);
    });

    document.querySelector("#home_images").addEventListener("click", () => {
        activate_images(api);
    });

    document.querySelector("#home_volumes").addEventListener("click", () => {
        activate_volumes(api);
    });

}

async function activate_home(api)
{
    const version = await api.version();
    set_text("#home_engine_version", `${version.Platform.Name} / ${version.Version}`);

    const info = await api.info();
    set_text("#home_name", info.Name);
    set_text("#home_arch", info.Architecture);
    set_text("#home_ncpu", info.NCPU);
    set_text("#home_memory", format_mem(info.MemTotal));

    activate_entry("home");
    const df = await api.disk_usage();
    let running = 0;
    for(const container of df.Containers) {
        if (container.State === "running") {
            running++;
        }
    }

    document.querySelector("#home_containers_running").textContent = running;
    document.querySelector("#home_containers_total").textContent = df.ContainerUsage.TotalCount;
    document.querySelector("#home_images_active").textContent = df.ImageUsage.ActiveCount;
    document.querySelector("#home_images_total").textContent = df.ImageUsage.TotalCount;
    document.querySelector("#home_volumes_active").textContent = df.VolumeUsage.ActiveCount;
    document.querySelector("#home_volumes_total").textContent = df.VolumeUsage.TotalCount;
}

async function activate_containers(api)
{
    activate_entry("containers");
    containers(api);
}

async function activate_images(api)
{
    activate_entry("images");
    images(api);
}

async function activate_volumes(api)
{
    activate_entry("volumes");
    volumes(api);
}

async function activate_system(api)
{
    activate_entry("system");
    engine_info(api);
    system_info(api);
    disk_usage(api);
}

async function startup() {
    const access_token = await login();


    const splash = document.querySelector("#splash");
    splash.classList.add("hidden");

    if (access_token) {
        const main = document.querySelector("#main");
        main.classList.remove("hidden");

        const api = new Api(access_token);
        init_home(api);
        document.querySelector("#menuentry_containers").addEventListener("click", () => {
            activate_containers(api);
        });
        document.querySelector("#menuentry_images").addEventListener("click", () => {
            activate_images(api);
        });

        document.querySelector("#menuentry_volumes").addEventListener("click", () => {
            activate_volumes(api);
        });
        document.querySelector("#volumes_new").addEventListener('click', async () => {
            try {
                const name = document.querySelector("#volumes_new_name");
                await api.create_volume(name.value);
                activate_volumes(api);
            }
            catch (ex) {
                console.log(ex);
            }
        });

        document.querySelector("#menuentry_system").addEventListener("click", () => {
            activate_system(api);
        });

        activate_home(api);
    }
    else {
        const login_failed = document.querySelector("#login_failed");
        login_failed.classList.remove("hidden");
    }
}


if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", startup());
} else {
  startup();
}
